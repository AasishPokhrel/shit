//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2013 LMH <lmh.0013@gmail.com>
//  Copyright (C) 2024 MatusGuy <matusguy@supertuxproject.org>
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

#include "badguy/mrbomb.hpp"

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "badguy/owl.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/constants.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

MrBomb::MrBomb(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/mr_bomb/mr_bomb.sprite", "left", "right"),
  m_state(MB_STATE_NORMAL),
  m_ticking_sound(),
  m_exploding_sprite(SpriteManager::current()->create("images/creatures/mr_bomb/ticking_glow/ticking_glow.sprite"))
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);

  SoundManager::current()->preload("sounds/explosion.wav");

  m_exploding_sprite->set_action("default", 1);
}

MrBomb::MrBomb(const ReaderMapping& reader, const std::string& sprite, const std::string& glow_sprite):
  WalkingBadguy(reader, sprite, "left", "right"),
  m_state(MB_STATE_NORMAL),
  m_ticking_sound(),
  m_exploding_sprite(SpriteManager::current()->create(glow_sprite))
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);

  SoundManager::current()->preload("sounds/explosion.wav");

  m_exploding_sprite->set_action("default", 1);
}

GameObjectTypes
MrBomb::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "classic", _("Classic") }
  };
}

std::string
MrBomb::get_default_sprite_name() const
{
  switch (m_type)
  {
    case CLASSIC:
      return "images/creatures/mr_bomb/old_bomb/old_bomb.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
MrBomb::collision_solid(const CollisionHit& hit)
{
  if (m_state == MB_STATE_TICKING) {
    if (hit.bottom)
      m_physic.set_velocity(0, 0);
    else
      kill_fall();

    update_on_ground_flag(hit);
    return;
  }

  WalkingBadguy::collision_solid(hit);
}

HitResponse
MrBomb::collision(MovingObject& object, const CollisionHit& hit)
{
  if (m_state == MB_STATE_TICKING)
  {
    auto player = dynamic_cast<Player*>(&object);
    if (player) return collision_player(*player, hit);
    auto badguy = dynamic_cast<BadGuy*>(&object);
    if (badguy) return collision_badguy(*badguy, hit);
  }

  if (is_grabbed())
    return FORCE_MOVE;

  return WalkingBadguy::collision(object, hit);
}

HitResponse
MrBomb::collision_player(Player& player, const CollisionHit& hit)
{
  if (m_state == MB_STATE_TICKING)
  {
    if (m_physic.get_velocity() != Vector())
      kill_fall();
    return ABORT_MOVE;
  }
  if (is_grabbed())
    return FORCE_MOVE;
  return BadGuy::collision_player(player, hit);
}

HitResponse
MrBomb::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (m_state == MB_STATE_TICKING)
  {
    if (m_physic.get_velocity() != Vector()) kill_fall();
    return ABORT_MOVE;
  } else if (m_state != MB_STATE_NORMAL) {
    return FORCE_MOVE;
  }
  return WalkingBadguy::collision_badguy(badguy, hit);
}

bool
MrBomb::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return WalkingBadguy::collision_squished(object);

  Player* player = dynamic_cast<Player*>(&object);
  if (player && player->is_invincible()) {
    player->bounce(*this);
    kill_fall();
    return true;
  }
  if (is_valid() && m_state != MB_STATE_TICKING) {
    trigger(player);
  }
  return true;
}

void
MrBomb::active_update(float dt_sec)
{
  if (m_state == MB_STATE_TICKING)
  {
    update_ticking(dt_sec);
    return;
  }

  WalkingBadguy::active_update(dt_sec);
}

void
MrBomb::draw(DrawingContext& context)
{
  m_sprite->draw(context.color(), get_pos(), m_layer, m_flip);

  if (m_state == MB_STATE_TICKING)
  {
    m_exploding_sprite->set_blend(Blend::ADD);
    m_exploding_sprite->draw(context.light(),
      get_pos() + Vector(get_bbox().get_width() / 2, get_bbox().get_height() / 2), m_layer, m_flip);
  }

  WalkingBadguy::draw(context);
}

void
MrBomb::trigger(Player* player)
{
  m_state = MB_STATE_TICKING;
  m_frozen = false;
  set_action("ticking", m_dir, 1);
  m_physic.set_velocity_x(0);

  if (player)
    player->bounce(*this);
  SoundManager::current()->play("sounds/squish.wav", get_pos());
  m_ticking_sound = SoundManager::current()->create_sound_source("sounds/fizz.wav");
  m_ticking_sound->set_position(get_pos());
  m_ticking_sound->set_looping(true);
  m_ticking_sound->set_gain(1.0f);
  m_ticking_sound->set_reference_distance(32);
  m_ticking_sound->play();
}

void
MrBomb::explode()
{
  remove_me();
  Sector::get().add<Explosion>(m_col.m_bbox.get_middle(),
    EXPLOSION_STRENGTH_DEFAULT);
  run_dead_script();
}

void
MrBomb::kill_fall()
{
  if (m_state == MB_STATE_TICKING)
    m_ticking_sound->stop();

  // Make the player let go before we explode, otherwise the player is holding
  // an invalid object.
  if (is_grabbed()) {
    Player* player = dynamic_cast<Player*>(m_owner);

    if (player)
      player->stop_grabbing();
  }

  if (is_valid()) {
    if (m_frozen)
      BadGuy::kill_fall();
    else
    {
      explode();
    }
  }
}

void
MrBomb::ignite()
{
  if (m_frozen)
    unfreeze();
  kill_fall();
}

void
MrBomb::grab(MovingObject& object, const Vector& pos, Direction dir_)
{
  Portable::grab(object, pos, dir_);

  if (m_state == MB_STATE_TICKING){
    // We actually face the opposite direction of Tux here to make the fuse more
    // visible instead of hiding it behind Tux.
    set_action("ticking", m_dir, Sprite::LOOPS_CONTINUED);
    set_colgroup_active(COLGROUP_DISABLED);
  }
  else if (m_frozen)
  {
    set_action("iced", dir_);
  }
  else if (dynamic_cast<Owl*>(&object))
    set_action(dir_);

  m_col.set_movement(pos - get_pos());
  m_physic.set_velocity(m_col.get_movement() * LOGICAL_FPS);
  m_dir = dir_;
  set_colgroup_active(COLGROUP_DISABLED);
}

void
MrBomb::ungrab(MovingObject& object, Direction dir_)
{
  auto player = dynamic_cast<Player*> (&object);
  if (m_frozen)
    BadGuy::ungrab(object, dir_);
  else
  {
    // Handle swimming state of the player.
    if (player && (player->is_swimming() || player->is_water_jumping()))
    {
      float swimangle = player->get_swimming_angle();
      m_physic.set_velocity(Vector(std::cos(swimangle) * 40.f, std::sin(swimangle) * 40.f) +
        player->get_physic().get_velocity());
    }
    // Handle non-swimming.
    else
    {
      if (player)
      {
        // Handle x-movement based on the player's direction and velocity.
        if (fabsf(player->get_physic().get_velocity_x()) < 1.0f)
          m_physic.set_velocity_x(0.f);
        else if ((player->m_dir == Direction::LEFT && player->get_physic().get_velocity_x() <= -1.0f)
          || (player->m_dir == Direction::RIGHT && player->get_physic().get_velocity_x() >= 1.0f))
          m_physic.set_velocity_x(player->get_physic().get_velocity_x()
            + (player->m_dir == Direction::LEFT ? -10.f : 10.f));
        else
          m_physic.set_velocity_x(player->get_physic().get_velocity_x()
            + (player->m_dir == Direction::LEFT ? -330.f : 330.f));
        // Handle y-movement based on the player's direction and velocity.
        m_physic.set_velocity_y(dir_ == Direction::UP ? -500.f :
          dir_ == Direction::DOWN ? 500.f :
          player->get_physic().get_velocity_x() != 0.f ? -200.f : 0.f);
      }
    }
  }
  set_colgroup_active(m_frozen ? COLGROUP_MOVING_STATIC : COLGROUP_MOVING);
  Portable::ungrab(object, dir_);
}

void
MrBomb::freeze()
{
  if (m_state != MB_STATE_TICKING) {
    m_state = MB_STATE_NORMAL;
    WalkingBadguy::freeze();
  }
}

bool
MrBomb::is_freezable() const
{
  return true;
}

bool
MrBomb::is_portable() const
{
  return (m_frozen || (m_state == MB_STATE_TICKING));
}

void
MrBomb::stop_looping_sounds()
{
  if (m_ticking_sound) {
    m_ticking_sound->stop();
  }
}

void
MrBomb::play_looping_sounds()
{
  if (m_state == MB_STATE_TICKING && m_ticking_sound) {
    m_ticking_sound->play();
  }
}

void
MrBomb::update_ticking(float dt_sec)
{
  m_exploding_sprite->set_action("exploding", 1);

  if (on_ground())
    m_physic.set_velocity_x(0);

  m_ticking_sound->set_position(get_pos());

  if (m_sprite->animation_done())
    kill_fall();
  else if (!is_grabbed())
    m_col.set_movement(m_physic.get_movement(dt_sec));
}

/* EOF */

