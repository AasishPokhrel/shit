//  SuperTux - Corrupted Granito - An "Evil" Granito
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//                2023 MatusGuy
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

#include "badguy/corrupted_granito.hpp"

#include "audio/sound_manager.hpp"
#include "sprite/sprite.hpp"

static const std::string CORRUPTED_GRANITO_SOUND = "sounds/hop.ogg";
static const float HORIZONTAL_SPEED = 220; /**< X-speed when jumping. */
static const float VERTICAL_SPEED = -450;   /**< Y-speed when jumping. */

CorruptedGranito::CorruptedGranito(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/granito/corrupted/corrupted_granito.sprite"),
  recover_timer(),
  state()
{
  parse_type(reader);

  SoundManager::current()->preload(CORRUPTED_GRANITO_SOUND);
}

CorruptedGranito::CorruptedGranito(const ReaderMapping& reader, int type) :
  CorruptedGranito(reader)
{
  m_type = type;
  on_type_change(TypeChange::INITIAL);
}

void
CorruptedGranito::initialize()
{
  // The initial state is JUMPING, because we might start airborne.
  state = JUMPING;
  set_action("jumping", m_dir);
}

void
CorruptedGranito::set_state(CorruptedGranitoState newState)
{
  if (newState == STANDING)
  {
    m_physic.set_velocity(0, 0);
    set_action("standing", m_dir);

    recover_timer.start(0.5);
  }
  else
  {
    if (newState == CHARGING)
    {
      set_action("charging", m_dir, 1);
    }
    else if (newState == JUMPING)
    {
      set_action("jumping", m_dir);
      m_physic.set_velocity_x(m_dir == Direction::LEFT ? -HORIZONTAL_SPEED : HORIZONTAL_SPEED);
      m_physic.set_velocity_y(VERTICAL_SPEED);
      SoundManager::current()->play( CORRUPTED_GRANITO_SOUND, get_pos());
    }
  }

  state = newState;
}

bool
CorruptedGranito::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return BadGuy::collision_squished(object);

  set_action("squished", m_dir);
  kill_squished(object);
  return true;
}

void
CorruptedGranito::collision_solid(const CollisionHit& hit)
{
  if (m_frozen || BadGuy::get_state() == STATE_BURNING)
  {
    BadGuy::collision_solid(hit);
    return;
  }

  // Default behaviour (i.e. stop at floor/walls) when squished.
  if (BadGuy::get_state() == STATE_SQUISHED)
    BadGuy::collision_solid(hit);

  // Ignore collisions while standing still.
  if (state != JUMPING)
    return;

  // Check if we hit the floor while falling.
  if (hit.bottom && m_physic.get_velocity_y() > 0)
    set_state(STANDING);

  // Check if we hit the roof while climbing.
  if (hit.top)
    m_physic.set_velocity_y(0);

  // Check if we hit left or right while moving in either direction.
  if (hit.left || hit.right)
  {
    m_dir = m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
    set_action("jumping", m_dir);
    m_physic.set_velocity_x(-0.25f*m_physic.get_velocity_x());
  }
}

HitResponse
CorruptedGranito::collision_badguy(BadGuy& , const CollisionHit& hit)
{
  // Behaviour for badguy collisions is the same as for collisions with solids.
  collision_solid(hit);

  return CONTINUE;
}

void
CorruptedGranito::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);

  // No change if frozen.
  if (m_frozen)
    return;

  // Charge when fully recovered.
  if (state == STANDING && recover_timer.check())
  {
    set_state(CHARGING);
    return;
  }

  // Jump as soon as charging animation completed.
  if (state == CHARGING && m_sprite->animation_done())
  {
    set_state(JUMPING);
    return;
  }
}

void
CorruptedGranito::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
  initialize();
}

bool
CorruptedGranito::is_freezable() const
{
  return m_type != GRANITO;
}

GameObjectTypes
CorruptedGranito::get_types() const
{
  return {
    { "granito", _("Granito") },
    { "skullyhop", _("Skullyhop") }
  };
}

std::string
CorruptedGranito::get_default_sprite_name() const
{
  switch (m_type)
  {
    case GRANITO:
      return "images/creatures/granito/corrupted/corrupted_granito.sprite";
    case SKULLYHOP:
      return "images/creatures/skullyhop/skullyhop.sprite";
  }
  return "images/creatures/granito/corrupted/corrupted_granito.sprite";
}

/* EOF */
