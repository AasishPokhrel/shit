//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "badguy/haywire.hpp"

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

namespace {

const float TIME_EXPLOSION = 5.0f;
const float STOMPED_TIME = 0.8f;
const float TIME_STUNNED = 0.5f;

const float NORMAL_WALK_SPEED = 80.0f;
const float EXPLODING_WALK_SPEED = 250.0f;
const float SKID_TIME = 0.3f;

} // namespace

Haywire::Haywire(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/haywire/haywire.sprite", "left", "right"),
  m_is_exploding(false),
  time_until_explosion(0.0f),
  is_stunned(false),
  time_stunned(0.0f),
  m_exploding_sprite(SpriteManager::current()->create("images/creatures/haywire/ticking_glow/ticking_glow.sprite")),
  m_jumping(false),
  m_skid_timer(),
  m_last_player_direction(Direction::LEFT),
  ticking(),
  grunting(),
  stomped_timer()
{
  walk_speed = NORMAL_WALK_SPEED;
  set_ledge_behavior(LedgeBehavior::SMART);

  SoundManager::current()->preload("sounds/explosion.wav");
}

Direction
Haywire::get_player_direction(const Player* player) const
{
  return (player->get_pos().x > get_pos().x) ? Direction::RIGHT : Direction::LEFT;
}

bool
Haywire::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return WalkingBadguy::collision_squished(object);

  auto player = dynamic_cast<Player*>(&object);
  if (player && player->is_invincible()) {
    player->bounce (*this);
    kill_fall();
    return true;
  }

  if (is_stunned) {
    if (player)
      player->bounce (*this);
    return true;
  }

  if (WalkingBadguy::is_frozen()) {
    WalkingBadguy::unfreeze();
  }

  if (!m_is_exploding) {
    m_last_player_direction = m_dir;
    start_exploding();
    stomped_timer.start(STOMPED_TIME);
  }

  time_stunned = TIME_STUNNED;
  is_stunned = true;
  m_physic.set_velocity_x(0.f);
  m_physic.set_acceleration_x(0.f);

  if (player)
    player->bounce (*this);

  return true;
}

void
Haywire::active_update(float dt_sec)
{
  auto* player = get_nearest_player();

  if (m_is_exploding) {
    ticking->set_position(get_pos());
    grunting->set_position(get_pos());
    if (dt_sec >= time_until_explosion) {
      kill_fall ();
      return;
    }
    else
      time_until_explosion -= dt_sec;
  }

  if (is_stunned) {
    if (time_stunned > dt_sec) {
      time_stunned -= dt_sec;
    }
    else { /* if (time_stunned <= dt_sec) */
      time_stunned = 0.f;
      is_stunned = false;
    }
  }

  if (m_is_exploding)
  {
    if (on_ground() && std::abs(m_physic.get_velocity_x()) > 40.f && player)
    {
      //jump over 1-tall roadblocks
      Rectf jump_box = get_bbox();
      jump_box.set_left(m_col.m_bbox.get_left() + (m_dir == Direction::LEFT ? -48.f : 38.f));
      jump_box.set_right(m_col.m_bbox.get_right() + (m_dir == Direction::RIGHT ? 48.f : -38.f));

      Rectf exception_box = get_bbox();
      exception_box.set_left(m_col.m_bbox.get_left() + (m_dir == Direction::LEFT ? -48.f : 38.f));
      exception_box.set_right(m_col.m_bbox.get_right() + (m_dir == Direction::RIGHT ? 48.f : -38.f));
      exception_box.set_top(m_col.m_bbox.get_top() - 32.f);
      exception_box.set_bottom(m_col.m_bbox.get_bottom() - 48.f);

      if (!Sector::get().is_free_of_statics(jump_box) && Sector::get().is_free_of_statics(exception_box))
      {
        m_physic.set_velocity_y(-325.f);
        m_jumping = true;
      }
      else
      {
        // Jump over gaps
        Rectf gap_box = get_bbox();
        gap_box.set_left(m_col.m_bbox.get_left() + (m_dir == Direction::LEFT ? -38.f : 26.f));
        gap_box.set_right(m_col.m_bbox.get_right() + (m_dir == Direction::LEFT ? -26.f : 38.f));
        gap_box.set_top(m_col.m_bbox.get_top());
        gap_box.set_bottom(m_col.m_bbox.get_bottom() + 28.f);

        if (Sector::get().is_free_of_statics(gap_box))
        {
          m_physic.set_velocity_y(-325.f);
          m_jumping = true;
        }
      }
    }

    // End of pathfinding.

	  if (stomped_timer.get_timeleft() < 0.05f) {
      if (m_jumping)
      {
        set_action("jump", m_dir, /* loops = */ 1);
        m_exploding_sprite->set_action("jump", /* loops = */ 1);
      }
      else if (!m_skid_timer.check() && m_skid_timer.started())
      {
        set_action((m_last_player_direction == Direction::LEFT) ? "skid-right" : "skid-left", /* loops = */ 1);
        m_exploding_sprite->set_action("skid", /* loops = */ 1);
      }
      else
      {
        set_action("ticking", m_last_player_direction, /* loops = */ -1);
        m_exploding_sprite->set_action("run", /* loops = */ -1);
      }
      walk_left_action = "ticking-left";
      walk_right_action = "ticking-right";
    }
    else {
      set_action("active", m_dir, /* loops = */ 1);
      walk_left_action = "active-left";
      walk_right_action = "active-right";
    }

    float target_velocity = 0.f;

    if (!m_frozen)
    {
      if (stomped_timer.get_timeleft() >= 0.05f)
      {
        target_velocity = 0.f;
      }
      else if (player && time_stunned == 0.0f)
      {
        /* Player is on the right or left. */
        Direction player_dir = get_player_direction(player);
        if (player_dir != m_last_player_direction)
        {
          m_skid_timer.start(SKID_TIME);
          m_last_player_direction = player_dir;
        }
        target_velocity = (player_dir == Direction::RIGHT) ? walk_speed : (-1.f) * walk_speed;
      }
    }
    else
    {
      target_velocity = 0.f;
    }
    WalkingBadguy::active_update(dt_sec, target_velocity, 3.f);
  }
  else
  {
    WalkingBadguy::active_update(dt_sec);
  }
}

void
Haywire::draw(DrawingContext& context)
{
  m_sprite->draw(context.color(), get_pos(), m_layer, m_flip);
  if (stomped_timer.get_timeleft() < 0.05f && m_is_exploding)
  {
    m_exploding_sprite->set_blend(Blend::ADD);
    m_exploding_sprite->draw(context.light(),
      get_pos() + (get_bbox().get_size().as_vector() / 2.0f), m_layer, m_flip);
  }
  WalkingBadguy::draw(context);
}

void
Haywire::kill_fall()
{
  if (m_is_exploding) {
    ticking->stop();
    grunting->stop();
  }
  if (is_valid()) {
    if (m_frozen)
      BadGuy::kill_fall();
    else
    {
      remove_me();
      Sector::get().add<Explosion>(m_col.m_bbox.get_middle(),
        EXPLOSION_STRENGTH_DEFAULT);
      run_dead_script();
    }
  }
}

bool
Haywire::is_freezable() const
{
  return true;
}

void
Haywire::ignite()
{
  if (m_frozen)
    unfreeze();
  kill_fall();
}

void
Haywire::freeze() {
  BadGuy::freeze();
  if (m_is_exploding) {
    stop_exploding();
  }
}

void
Haywire::start_exploding()
{
  set_walk_speed (EXPLODING_WALK_SPEED);
  set_ledge_behavior(LedgeBehavior::FALL);
  time_until_explosion = TIME_EXPLOSION;
  m_is_exploding = true;

  ticking = SoundManager::current()->create_sound_source("sounds/fizz.wav");
  ticking->set_position(get_pos());
  ticking->set_looping(true);
  ticking->set_reference_distance(32);
  ticking->play();
  grunting = SoundManager::current()->create_sound_source("sounds/grunts.ogg");
  grunting->set_position(get_pos());
  grunting->set_looping(true);
  grunting->set_reference_distance(32);
  grunting->play();
}

void
Haywire::stop_exploding()
{
  walk_left_action = "left";
  walk_right_action = "right";
  set_walk_speed(NORMAL_WALK_SPEED);
  set_ledge_behavior(LedgeBehavior::SMART);
  time_until_explosion = 0.0f;
  m_is_exploding = false;

  if (ticking)
    ticking->stop();

  if (grunting)
    grunting->stop();
}

void Haywire::stop_looping_sounds()
{
  if (ticking) {
    ticking->stop();
  }
  if (grunting) {
    grunting->stop();
  }
}

void Haywire::play_looping_sounds()
{
  if (m_is_exploding) {
    if (ticking) {
      ticking->play();
    }
    if (grunting) {
      grunting->play();
    }
  }
}

void
Haywire::collision_solid(const CollisionHit& hit)
{
  WalkingBadguy::collision_solid(hit);

  m_jumping = false;
}

HitResponse Haywire::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (m_is_exploding)
  {
    badguy.kill_fall();
    return FORCE_MOVE;
  }
  if (m_frozen)
    return FORCE_MOVE;

  return WalkingBadguy::collision_badguy(badguy, hit);
}

/* EOF */
