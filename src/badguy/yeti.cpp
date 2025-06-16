//  SuperTux - Boss "Yeti"
//  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "badguy/yeti.hpp"

#include <cmath>

#include "audio/sound_manager.hpp"
#include "badguy/bouncing_snowball.hpp"
#include "badguy/yeti_stalactite.hpp"
#include "editor/editor.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "object/bigsnowball.hpp"
#include "object/camera.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/surface.hpp"

namespace
{
const float ANNOUNCE_INTERVAL = 0.1f;

const float RUN_VX = 350; /**< Horizontal speed while running. */
const float RUN_PINCH_VX = 400; /**< Horizontal speed while running. */

const float JUMP_UP_VY = -775; /**< Vertical speed while jumping on the dais. */
const float STOMP_VY = -300; /**< Vertical speed while stomping on the dais. */

const float BALL_SPEED = 1.3f;
const float BALL_PINCH_SPEED = 1.5f;

const float BEFORE_WAIT = 1.f;
const float BALL_WAIT = 0.8f;
const float STOMP_WAIT = 0.25f; /**< Time we stay on the dais before jumping again. */
const float SAFE_TIME = 1.f; /**< The time we are safe when Tux just hit us. */

const float JUMP_SPACE = 13.5f * 32.f; /**< Distance between the jump position and the stand position. */

const float TUX_GRAB_DISTANCE = 100.f;
}

Yeti::Yeti(const ReaderMapping& reader) :
  Boss(reader, "images/creatures/yeti/yeti.sprite"),
  m_state(ANNOUNCE),
  m_next_state(RUN),
  m_state_timer(),
  m_safe_timer(),
  m_attacked(false),
  m_attack_count(),
  m_left_stand_x(),
  m_right_stand_x(),
  m_left_jump_x(),
  m_right_jump_x(),
  m_just_hit(),
  m_pinch_announced(),
  m_grabbed_tux()
{
  reader.get("hud-icon", m_hud_icon, "images/creatures/yeti/hudlife.png");
  m_hud_head = Surface::from_file(m_hud_icon);

  SoundManager::current()->preload("sounds/thud.ogg");
  SoundManager::current()->preload("sounds/yeti_gna.wav");
  SoundManager::current()->preload("sounds/yeti_roar.wav");
}

void
Yeti::initialize()
{
  recalculate_pos();

  // Force all players to face the yeti because this way it looks epic...
  // ... i think
  bool playerdir = (invert_dir(m_dir) == Direction::RIGHT);
  for (Player* player : Sector::get().get_players())
  {
    player->set_dir(playerdir);
  }

  m_next_state = RUN;
  announce(false);
}

void
Yeti::recalculate_pos()
{
  float sectorw = Sector::get().get_width();

  if (m_dir == Direction::RIGHT) {
    m_left_stand_x = m_col.m_bbox.get_left();
    m_right_stand_x = sectorw - m_left_stand_x - m_col.m_bbox.get_width();
  } else {
    m_right_stand_x = m_col.m_bbox.get_left();
    m_left_stand_x = sectorw - m_right_stand_x;
  }

  m_left_jump_x = m_left_stand_x + JUMP_SPACE;
  m_right_jump_x = m_right_stand_x - JUMP_SPACE;
}

void
Yeti::draw(DrawingContext& context)
{
  // We blink when we are safe.
  if (m_safe_timer.started() && size_t(g_game_time * 40) % 2)
    return;

  Boss::draw(context);
}

void
Yeti::active_update(float dt_sec)
{
  Boss::boss_update(dt_sec);

  Vector grab_pos = get_bbox().get_middle();
  float push_distance;
  auto player = get_nearest_player();

  push_distance = player ? glm::length(grab_pos - player->get_bbox().get_middle()) : 0.f;

  if (player && on_ground() && is_idle() &&
      push_distance <= TUX_GRAB_DISTANCE && m_physic.get_velocity_x() == 0.f)
  {
    throw_tux();
  }

  switch (m_state) {
    case ANNOUNCE:
      if (m_next_state == ANNOUNCE)
        break;

      if (m_state_timer.check())
      {
        if (m_next_state == RUN)
        {
          run(false);
          jump(STOMP_VY);
        }
        else
        {
          m_pinch_announced = true;
          idle(false, BEFORE_WAIT);
        }
      }

      if (m_sprite->animation_done())
      {
        set_action("stand", m_dir);
        m_state_timer.start(ANNOUNCE_INTERVAL);
      }

      break;

    case JUMP:
      if (m_sprite->animation_done())
        set_action("fall", m_dir);

      if (on_ground())
      {
        switch (m_next_state)
        {
          case RUN:
            run(true);
            break;

          case IDLE:
            idle(true);
            break;

          default:
            break;
        }
      }

      break;

    case RUN:
      if ((m_dir == Direction::RIGHT && math::in_bounds(get_pos().x, m_right_jump_x, m_right_jump_x + 64.f)) ||
          (m_dir == Direction::LEFT && math::in_bounds(get_pos().x, m_left_jump_x - 64.f, m_left_jump_x)))
      {
        m_next_state = IDLE;
        jump(JUMP_UP_VY);
      }

      break;

    case IDLE:
      if (m_sprite->animation_done())
      {
        m_dir = invert_dir(m_dir);
        set_action("stand", m_dir);
        m_state_timer.start(BEFORE_WAIT);
      }

      if (m_state_timer.check())
      {
        // If the player is dead, laugh at him. Forever.
        if (!player)
        {
          m_next_state = ANNOUNCE;
          announce(true);
          break;
        }

        if (!m_pinch_announced && m_pinch_mode)
        {
          announce(false);
          break;
        }

        switch (m_next_state)
        {
          case RUN:
            run(false);
            jump(STOMP_VY);
            break;

          case THROW:
            throw_snowball();
            break;

          case STOMP:
            stomp();
            break;

          case THROW_BIG:
            throw_big_snowball();
            break;

          default:
            break;
        }
      }

      break;

    case THROW:
      if (!m_attacked && m_sprite->get_current_frame() == 5)
      {
        summon_snowball();
        m_attacked = true;
        m_attack_count++;
        break;
      }

      if (m_sprite->animation_done())
      {
        if (m_attack_count >= (m_pinch_mode ? 3 : 2))
        {
          m_next_state = STOMP;
          m_attack_count = 0;
        }
        else
          m_next_state = THROW;

        m_attacked = false;
        idle(false, BALL_WAIT);
      }

      break;

    case STOMP:
      if (m_attacked && m_sprite->animation_done())
      {
        if (m_attack_count >= 3)
        {
          m_next_state = m_pinch_mode ? THROW_BIG : RUN;
          m_attack_count = 0;
        }
        else
          m_next_state = STOMP;

        m_attacked = false;
        idle(false, STOMP_WAIT);
      }

      break;

    case THROW_BIG:
      if (!m_attacked && m_sprite->get_current_frame() == 16)
      {
        summon_big_snowball();
        m_attacked = true;
        m_attack_count++;
        break;
      }

      if (m_sprite->animation_done())
      {
        if (m_attack_count >= (m_lives == 1 ? 2 : 1))
        {
          m_next_state = RUN;
          m_attack_count = 0;
        }
        else
          m_next_state = THROW_BIG;

        m_attacked = false;
        idle(false, BALL_WAIT);
      }

      break;

    case THROW_TUX:
      if (!m_grabbed_tux && push_distance < TUX_GRAB_DISTANCE && player)
      {
        player->get_physic().set_velocity(5.f * (grab_pos - player->get_bbox().get_middle()));

        if (push_distance < 10.f)
        {
          m_state_timer.start(BALL_WAIT / 2.f);
          m_grabbed_tux = true;

          set_action("push", m_dir);
          player->get_physic().set_velocity(m_dir == Direction::RIGHT ? 600.f : -600.f, -200.f);
        }
      }

      if (m_state_timer.check())
      {
        set_group(COLGROUP_MOVING);
        m_physic.enable_gravity(true);
        m_grabbed_tux = false;

        m_next_state = THROW;
        idle(false, BALL_WAIT);
      }

      break;

    case DIZZY:
      if (m_state_timer.check() ||
          might_fall(BadGuy::s_normal_max_drop_height))
      {
        bust();
      }

      break;

    case BUSTED:
      if (m_state_timer.check())
        run_dead_script();

      break;

    default:
      break;
  }

  m_col.set_movement(m_physic.get_movement(dt_sec));
}

void
Yeti::announce(bool taunt)
{
  m_state = ANNOUNCE;
  set_action(taunt ? "taunt" : "rage", m_dir);
  m_state_timer.stop();

  SoundManager::current()->play("sounds/yeti_roar.wav", get_pos());
}

void
Yeti::run(bool change_state)
{
  if (change_state)
  {
    m_state = RUN;
    set_action("walk", m_dir);
    m_state_timer.start(BEFORE_WAIT);
  }

  if (m_pinch_mode)
    m_physic.set_velocity_x(RUN_PINCH_VX * (m_dir == Direction::RIGHT ? 1.f : -1.f));
  else
    m_physic.set_velocity_x(RUN_VX * (m_dir == Direction::RIGHT ? 1.f : -1.f));
}

void
Yeti::jump(float velocity)
{
  m_state = JUMP;
  set_action("jump", m_dir);

  m_physic.set_velocity_y(velocity);
}

void
Yeti::idle(bool stomp, float waitduration)
{
  m_state = IDLE;
  set_action(stomp ? "stomp" : "stand", m_dir);
  m_just_hit = false;

  m_state_timer.stop();

  if (stomp)
    m_next_state = THROW;
  else
    m_state_timer.start(waitduration);

  m_physic.set_velocity_x(0);
}

void
Yeti::throw_snowball()
{
  m_state = THROW;
  set_action("throw", m_dir);

  m_state_timer.start(STOMP_WAIT / (m_pinch_mode ? 1.2f : 1.f));
}

void
Yeti::throw_big_snowball()
{
  m_state = THROW_BIG;
  set_action("big-throw", m_dir, 0);

  m_state_timer.start(BALL_WAIT);
}

void
Yeti::stomp()
{
  m_state = STOMP;
  set_action("leap", m_dir);

  m_physic.set_velocity_y(STOMP_VY);
  SoundManager::current()->play("sounds/yeti_gna.wav", get_pos());
}

void
Yeti::throw_tux()
{
  m_state = THROW_TUX;
  set_action("grab", m_dir, 1);
  m_attacked = false;
  m_attack_count = 0;

  m_state_timer.stop();
  m_physic.enable_gravity(false);
  set_group(COLGROUP_DISABLED);
}

void
Yeti::turn_dizzy()
{
  // We're dead.
  Direction old_dir = m_dir;
  m_dir = invert_dir(old_dir);

  m_physic.set_velocity((m_dir == Direction::RIGHT ? RUN_VX : -RUN_VX) / 5, 0);
  set_action("dizzy", old_dir);

  // NOTE: What?
  // Set the badguy layer to be above the foremost, so that
  // this does not reveal secret tilemaps:
  //m_layer = Sector::get().get_foremost_opaque_layer() + 1;

  m_state = DIZZY;
  set_group(COLGROUP_MOVING_ONLY_STATIC);

  m_state_timer.start(4.f);
}

void
Yeti::bust() // You see, it's funny because
{
  m_state = BUSTED;
  set_action("busted", invert_dir(m_dir));

  m_physic.set_velocity_x(0);
  Sector::get().get_camera().shake(.1f, 0, 16.f);
  m_state_timer.start(1.5f);
}

bool
Yeti::is_idle()
{
  return m_state == IDLE ||
         m_state == THROW ||
         m_state == THROW_BIG ||
         m_state == STOMP;
}

bool
Yeti::collision_squished(MovingObject& object)
{
  if (m_state != YetiState::JUMP && m_state != YetiState::RUN && m_state != YetiState::STOMP) {
    return false;
  }

  kill_squished(object);
  return true;
}

HitResponse
Yeti::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  badguy.kill_fall();
  return FORCE_MOVE;
}

void
Yeti::kill_squished(GameObject& object)
{
  auto player = dynamic_cast<Player*>(&object);
  if (player) {
    player->bounce(*this);
    take_hit(*player);
  }
}

void
Yeti::take_hit(Player& )
{
  if (m_safe_timer.started() || m_just_hit ||
    std::abs(m_left_stand_x - get_pos().x) < 80 || std::abs(m_right_stand_x - get_pos().x) < 80)
    return;

  m_just_hit = true;

  SoundManager::current()->play("sounds/yeti_roar.wav", get_pos());
  m_lives--;

  if (m_lives <= 0)
    turn_dizzy();
  else
    m_safe_timer.start(SAFE_TIME);
}

void
Yeti::kill_fall()
{
  // Shooting bullets or being invincible won't prevent this action.
}

void
Yeti::drop_stalactite()
{
  SoundManager::current()->play("sounds/thud.ogg", get_pos());

  // Make a stalactite fall down and shake the camera a bit.
  Sector::get().get_camera().shake(.1f, 0, 20.f);

  auto player = get_nearest_player();
  if (!player) return;

  for (auto& stalactite : Sector::get().get_objects_by_type<YetiStalactite>())
  {
    if (stalactite.is_hanging()) {
      if (!m_pinch_mode) {
        // Drop stalactites within 3 units of player, going out with each jump.
        float distancex = fabsf(stalactite.get_bbox().get_middle().x - player->get_bbox().get_middle().x);
        if (distancex < static_cast<float>(m_attack_count) * 32.0f) {
          stalactite.start_shaking();
        }
      }
      else { /* if (hitpoints < 3) */
        // Drop every 3rd pair of stalactites.
        if ((((static_cast<int>(stalactite.get_pos().x) + 16) / 64) % 3) == (m_attack_count % 3))
          stalactite.start_shaking();
      }
    }
  }
}

void
Yeti::summon_snowball()
{
  Vector bs_pos = get_pos() + Vector(m_dir == Direction::LEFT ? -32.f : (get_bbox().get_width() + 1.f), 0.f);
  Sector::get().add<BouncingSnowball>(bs_pos, m_dir, 150.f * (m_pinch_mode ? BALL_PINCH_SPEED : BALL_SPEED));
}

void
Yeti::summon_big_snowball()
{
  auto& ball = Sector::get().add<BigSnowball>(Vector(0,0), m_dir, true);

  float x = m_dir == Direction::LEFT ? get_bbox().get_left() - 32.f : get_bbox().get_right();
  Vector pos(x, get_bbox().get_top());
  ball.set_pos(pos);
}

void
Yeti::collision_solid(const CollisionHit& hit)
{
  update_on_ground_flag(hit);
  if (!hit.bottom)
    return;

  m_physic.set_velocity_y(0);
  switch (m_state)
  {
    case STOMP:
      if (m_attacked)
        break;

      set_action("stomp", m_dir);
      m_attack_count++;
      m_attacked = true;
      drop_stalactite();

      break;

    default:
      break;
  }
}

void
Yeti::add_snow_explosions()
{
  // TODO: WAOW! This is terrible! Remove this!
  /*
  for (int i = 0; i < SNOW_EXPLOSIONS_COUNT; i++) {
    Vector pos = get_pos();
    Vector velocity(SNOW_EXPLOSIONS_VX * graphicsRandom.randf(0.5f, 2.0f) * (graphicsRandom.rand(2) ? 1.0f : -1.0f),
                    SNOW_EXPLOSIONS_VY * graphicsRandom.randf(0.5f, 2.0f));
    pos.x += static_cast<float>(m_sprite->get_width()) / 2.0f;
    pos.x += static_cast<float>(m_sprite->get_width()) * graphicsRandom.randf(0.3f, 0.5f) * ((velocity.x > 0) ? 1.0f : -1.0f);
    pos.y += static_cast<float>(m_sprite->get_height()) * graphicsRandom.randf(-0.3f, 0.3f);
    velocity.x += m_physic.get_velocity_x();
    Sector::get().add<SnowExplosionParticle>(pos, velocity);
  }
  */
}

Yeti::SnowExplosionParticle::SnowExplosionParticle(const Vector& pos, const Vector& velocity)
  : BadGuy(pos, (velocity.x > 0) ? Direction::RIGHT : Direction::LEFT, "images/objects/bullets/icebullet.sprite")
{
  m_physic.set_velocity(velocity);
  m_physic.enable_gravity(true);
  set_state(STATE_FALLING);
  m_layer = Sector::get().get_foremost_opaque_layer() + 1;
}

std::vector<Direction>
Yeti::get_allowed_directions() const
{
  return { Direction::LEFT, Direction::RIGHT };
}

/* EOF */
