//  SuperTux - Badguy "Snail"
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

#include "badguy/snail.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "badguy/owl.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "sprite/sprite.hpp"
#include "supertux/constants.hpp"
#include "supertux/sector.hpp"

namespace {

const float SNAIL_KICK_SPEED = 500;
const int MAX_SNAIL_SQUISHES = 10;
const float SNAIL_KICK_SPEED_Y = -500; /**< Y-velocity gained when kicked. */

const float SNAIL_GUARD_DELAY = 5.f; /**< Time in-between corrupted snail guard states (seconds). */
const float SNAIL_GUARD_TIME = 3.f; /**< Duration of corrupted snail guard states (seconds). */

} // namespace

Snail::Snail(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/snail/snail.sprite", "left", "right"),
  state(STATE_NORMAL),
  kicked_delay_timer(),
  flat_timer(),
  m_guard_timer(),
  m_guard_end_timer(),
  squishcount(0)
{
  parse_type(reader);

  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
  SoundManager::current()->preload("sounds/iceblock_bump.wav");
  SoundManager::current()->preload("sounds/stomp.wav");
  SoundManager::current()->preload("sounds/kick.wav");
  SoundManager::current()->preload("sounds/dartfire.wav"); // TODO: Specific sounds for snail guard state.
}

void
Snail::initialize()
{
  WalkingBadguy::initialize();
  be_normal();
}

GameObjectTypes
Snail::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "corrupted", _("Corrupted") }
  };
}

std::string
Snail::get_default_sprite_name() const
{
  switch (m_type)
  {
    case CORRUPTED:
      return "images/creatures/snail/corrupted/corrupted_snail.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
Snail::be_normal()
{
  if (m_type == Type::CORRUPTED)
    m_guard_timer.start(SNAIL_GUARD_DELAY);

  if (state == STATE_NORMAL) return;

  state = STATE_NORMAL;
  WalkingBadguy::initialize();
}

void
Snail::be_guard()
{
  if (m_type != Type::CORRUPTED) return;

  state = STATE_GUARD_SHAKE;
  set_action("shake", m_dir, /* loops = */ 1);
  m_physic.set_velocity_x(0);
}

void
Snail::be_flat()
{
  state = STATE_FLAT;
  set_action("flat", m_dir, /* loops = */ -1);

  m_physic.set_velocity(0, 0);

  flat_timer.start(4);
}

void
Snail::be_grabbed()
{
  state = STATE_GRABBED;
  set_action("flat", m_dir, /* loops = */ -1);
}

void
Snail::be_kicked(bool upwards)
{
  if(upwards)
    state = STATE_KICKED_DELAY;
  else
    state = STATE_KICKED;
  set_action("flat", m_dir, /* loops = */ -1);

  m_physic.set_velocity(m_dir == Direction::LEFT ? -SNAIL_KICK_SPEED : SNAIL_KICK_SPEED, 0);

  // Start a timer to delay addition of upward movement until we are (hopefully) out from under the player.
  if (upwards)
    kicked_delay_timer.start(0.05f);
}

void
Snail::wake_up()
{
  state = STATE_WAKING;
  set_action("waking", m_dir, /* loops = */ 1);
}

bool
Snail::can_break() const
{
  return state == STATE_KICKED;
}

void
Snail::active_update(float dt_sec)
{
  if (state == STATE_GRABBED || is_grabbed())
    return;
  
  if (m_frozen)
  {
    BadGuy::active_update(dt_sec);
    return;
  }

  if (m_guard_timer.check() && state == STATE_NORMAL)
    be_guard();

  switch (state) {

    case STATE_NORMAL:
      WalkingBadguy::active_update(dt_sec);
      return;

    case STATE_GUARD_SHAKE:
      if (m_sprite->animation_done())
      {
        state = STATE_GUARD;
        set_action("guard", m_dir);
        SoundManager::current()->play("sounds/dartfire.wav", get_pos()); // TODO: Specific sounds for snail guard state.
        m_guard_end_timer.start(SNAIL_GUARD_TIME);
      }
      break;

    case STATE_GUARD:
      if (m_guard_end_timer.check())
      {
        state = STATE_GUARD_RETRACT;
        set_action("retract", m_dir, /* loops = */ 1);
        SoundManager::current()->play("sounds/dartfire.wav", get_pos()); // TODO: Specific sounds for snail guard state.
      }
      break;

    case STATE_FLAT:
      if (flat_timer.check())
        wake_up();
      break;

    case STATE_GUARD_RETRACT:
    case STATE_WAKING:
      if (m_sprite->animation_done())
        be_normal();
      break;

    case STATE_KICKED_DELAY:
      if (kicked_delay_timer.check()) {
        m_physic.set_velocity(m_dir == Direction::LEFT ? -SNAIL_KICK_SPEED : SNAIL_KICK_SPEED, SNAIL_KICK_SPEED_Y);
        state = STATE_KICKED;
      }
      break;

    case STATE_KICKED:
      m_physic.set_velocity_x(m_physic.get_velocity_x() * powf(0.99f, dt_sec/0.02f));
      if (on_ground() && (fabsf(m_physic.get_velocity_x()) < walk_speed)) be_normal();
      break;

    case STATE_GRABBED:
      break;
  }

  BadGuy::active_update(dt_sec);

  if (m_ignited)
    remove_me();
}

bool
Snail::is_freezable() const
{
  return true;
}

bool
Snail::is_snipable() const
{
  return state != STATE_KICKED &&
         state != STATE_GUARD;
}

void
Snail::collision_solid(const CollisionHit& hit)
{
  if (m_frozen)
  {
    WalkingBadguy::collision_solid(hit);
    return;
  }

  switch (state)
  {
    case STATE_NORMAL:
      WalkingBadguy::collision_solid(hit);
      return;
    case STATE_KICKED:
      if (hit.left || hit.right) {
        SoundManager::current()->play("sounds/iceblock_bump.wav", get_pos());

        if ( ( m_dir == Direction::LEFT && hit.left ) || ( m_dir == Direction::RIGHT && hit.right) ){
          m_dir = (m_dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT;
          set_action("flat", m_dir, /* loops = */ -1);

          m_physic.set_velocity(-m_physic.get_velocity_x(), -std::abs(m_physic.get_velocity_x()));
        }
      }
      [[fallthrough]];
    case STATE_GUARD_SHAKE:
    case STATE_GUARD:
    case STATE_GUARD_RETRACT:
    case STATE_FLAT:
    case STATE_KICKED_DELAY:
    case STATE_WAKING:
      if (hit.top || hit.bottom) {
        m_physic.set_velocity_y(0);
      }
      break;

    case STATE_GRABBED:
      break;
  }

  update_on_ground_flag(hit);

}

HitResponse
Snail::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (m_frozen)
    return WalkingBadguy::collision_badguy(badguy, hit);

  switch (state) {
    case STATE_NORMAL:
      return WalkingBadguy::collision_badguy(badguy, hit);
    case STATE_GUARD_SHAKE:
    case STATE_GUARD:
    case STATE_GUARD_RETRACT:
      return BadGuy::collision_badguy(badguy, hit);
    case STATE_FLAT:
    case STATE_KICKED_DELAY:
    case STATE_WAKING:
      return FORCE_MOVE;
    case STATE_KICKED:
      badguy.kill_fall();
      return FORCE_MOVE;
    default:
      assert(false);
  }

  return ABORT_MOVE;
}

HitResponse
Snail::collision_player(Player& player, const CollisionHit& hit)
{
  if (m_frozen)
    return BadGuy::collision_player(player, hit);

  if (state == STATE_GUARD)
    return WalkingBadguy::collision_player(player, hit); // Hurt player on collision.

  // Handle kicks from left or right side.
  if ((state == STATE_WAKING || state == STATE_FLAT) && (hit.left || hit.right)) {
    if (hit.left) {
      m_dir = Direction::RIGHT;
    } else if (hit.right) {
      m_dir = Direction::LEFT;
    }
    SoundManager::current()->play("sounds/kick.wav", get_pos());
    player.kick();
    be_kicked(false);
    return FORCE_MOVE;
  }

  return BadGuy::collision_player(player, hit);
}

bool
Snail::collision_squished(MovingObject& object)
{
  if (m_frozen || state == STATE_GUARD)
    return WalkingBadguy::collision_squished(object);

  Player* player = dynamic_cast<Player*>(&object);
  if (player && (player->is_invincible() || player->m_does_buttjump)) {
    kill_fall();
    player->bounce(*this);
    return true;
  }

  switch (state) {
    case STATE_NORMAL:
      [[fallthrough]];
    case STATE_KICKED:
      squishcount++;
      if (squishcount >= MAX_SNAIL_SQUISHES) {
        kill_fall();
        return true;
      }
      SoundManager::current()->play("sounds/stomp.wav", get_pos());
      be_flat();
      break;

    case STATE_FLAT:
    case STATE_WAKING:
      SoundManager::current()->play("sounds/kick.wav", get_pos());
      {
        if (object.get_pos().x < get_pos().x) {
          m_dir = Direction::RIGHT;
        } else {
          m_dir = Direction::LEFT;
        }
      }
      be_kicked(false);
      break;

    default:
      break;
  }

  if (player) player->bounce(*this);
  return true;
}

void
Snail::grab(MovingObject& object, const Vector& pos, Direction dir_)
{
  Portable::grab(object, pos, dir_);
  if (m_frozen)
    BadGuy::grab(object, pos, dir_);
  m_col.set_movement(pos - get_pos());
  m_physic.set_velocity(m_col.get_movement() * LOGICAL_FPS);
  m_dir = dir_;
  if (!m_frozen)
  {
    set_action("flat", dir_, /* loops = */ -1);
    be_grabbed();
    flat_timer.stop();
  }
  set_colgroup_active(COLGROUP_DISABLED);
}

void
Snail::ungrab(MovingObject& object, Direction dir_)
{
  if (!m_frozen)
  {
    const Player* player = dynamic_cast<Player*>(&object);
    const Owl* owl = dynamic_cast<Owl*>(&object);

    if (player)
    {
      SoundManager::current()->play("sounds/kick.wav", get_pos());
      if (!player->is_swimming() && !player->is_water_jumping())
      {
        switch (dir_)
        {
        case Direction::UP:
          if (std::abs(player->get_velocity().x) < 4.f) {
            be_flat();
            m_physic.set_velocity_y(SNAIL_KICK_SPEED_Y);
          }
          else {
            be_kicked(true);
          }
          break;
        case Direction::LEFT:
        case Direction::RIGHT:
          m_dir = dir_;
          be_kicked(false);
          break;
        case Direction::DOWN:
          m_dir = player->m_dir;
          be_kicked(false);
          m_physic.set_velocity_y(500.f);
          break;
        default:
          break;
        }
      }
      else
      {
        float swimangle = player->get_swimming_angle();
        m_col.m_bbox.move(Vector(std::cos(swimangle) * 48.f, std::sin(swimangle) * 48.f));
        be_kicked(false);
        m_physic.set_velocity(SNAIL_KICK_SPEED * 1.5f * Vector(std::cos(swimangle), std::sin(swimangle)));
        m_dir = m_physic.get_velocity_x() > 0.f ? Direction::RIGHT : Direction::LEFT;
      }
    }
    else if (owl) {
      be_kicked(false);
    }
  }
  else
    BadGuy::ungrab(object, dir_);
  set_colgroup_active(m_frozen ? COLGROUP_MOVING_STATIC : COLGROUP_MOVING);
  Portable::ungrab(object, dir_);
}

bool
Snail::is_portable() const
{
  return (state == STATE_FLAT || m_frozen) && !m_ignited;
}

/* EOF */
