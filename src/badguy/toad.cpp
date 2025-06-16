//  Toad - A jumping toad
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

#include "badguy/toad.hpp"

#include "audio/sound_manager.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"

namespace {
const float VERTICAL_SPEED = -450;   /**< Vertical speed when jumping. */
const float HORIZONTAL_SPEED = 320; /**< Horizontal speed when jumping. */
const float TOAD_RECOVER_TIME = 0.5; /**< Time to stand still before starting a (new) jump. */
static const std::string HOP_SOUND = "sounds/hop.ogg";
}

Toad::Toad(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/toad/toad.sprite"),
  recover_timer(),
  state()
{
  SoundManager::current()->preload(HOP_SOUND);
}

void
Toad::initialize()
{
  // The initial state is JUMPING, because we might start airborne.
  state = JUMPING;
  set_action("jumping", m_dir);
}

void
Toad::set_state(ToadState newState)
{
  if (newState == IDLE) {
    m_physic.set_velocity(0, 0);
    if (!m_frozen)
      set_action("idle", m_dir);

    recover_timer.start(TOAD_RECOVER_TIME);
  } else
    if (newState == JUMPING) {
      set_action("jumping", m_dir);
      m_physic.set_velocity(m_dir == Direction::LEFT ? -HORIZONTAL_SPEED : HORIZONTAL_SPEED, VERTICAL_SPEED);
      SoundManager::current()->play( HOP_SOUND, get_pos());
    } else
      if (newState == FALLING) {
        Player* player = get_nearest_player();
        // Face the player.
        if (player && (player->get_bbox().get_right() < m_col.m_bbox.get_left()) && (m_dir == Direction::RIGHT)) m_dir = Direction::LEFT;
        if (player && (player->get_bbox().get_left() > m_col.m_bbox.get_right()) && (m_dir == Direction::LEFT)) m_dir = Direction::RIGHT;
        set_action("idle", m_dir);
      }

  state = newState;
}

bool
Toad::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return BadGuy::collision_squished(object);
  set_action("squished", m_dir);
  kill_squished(object);
  return true;
}

void
Toad::collision_solid(const CollisionHit& hit)
{
  // Default behavior when frozen.
  if (m_frozen || BadGuy::get_state() == STATE_BURNING)
  {
    BadGuy::collision_solid(hit);
    return;
  }

  // Default behaviour (i.e. stop at floor/walls) when squished.
  if (BadGuy::get_state() == STATE_SQUISHED) {
    BadGuy::collision_solid(hit);
    return;
  }

  // Ignore collisions while standing still.
  if (state == IDLE) {
    return;
  }

  // Check if we hit left or right while moving in either direction.
  if (((m_physic.get_velocity_x() < 0) && hit.left) || ((m_physic.get_velocity_x() > 0) && hit.right)) {
    /*
      dir = dir == LEFT ? RIGHT : LEFT;
      if (state == JUMPING) {
      sprite->set_action(dir == LEFT ? "jumping-left" : "jumping-right");
      } else {
      sprite->set_action(dir == LEFT ? "idle-left" : "idle-right");
      }
    */
    m_physic.set_velocity_x(-0.25f*m_physic.get_velocity_x());
  }

  // Check if we hit the floor while falling.
  if ((state == FALLING) && hit.bottom) {
    set_state(IDLE);
    return;
  }

  // Check if we hit the roof while climbing.
  if ((state == JUMPING) && hit.top) {
    m_physic.set_velocity_y(0);
  }

}

HitResponse
Toad::collision_badguy(BadGuy& , const CollisionHit& hit)
{
  // Behaviour for badguy collisions is the same as for collisions with solids.
  collision_solid(hit);

  return CONTINUE;
}

void
Toad::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);


  // Change sprite when we are falling and not frozen.
  if ((state == JUMPING) && (m_physic.get_velocity_y() > 0) && !m_frozen) {
    set_state(FALLING);
    return;
  }

  // Jump when fully recovered and if not frozen.
  if ((state == IDLE) && (recover_timer.check() && !m_frozen)) {
    set_state(JUMPING);
    return;
  }

}

void
Toad::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
  initialize();
}

bool
Toad::is_freezable() const
{
  return true;
}

/* EOF */
