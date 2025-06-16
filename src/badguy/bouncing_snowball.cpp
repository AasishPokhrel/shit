//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "badguy/bouncing_snowball.hpp"

#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"

#include <algorithm>

static const float JUMPSPEED = -450;
static const float BSNOWBALL_WALKSPEED = 80;

BouncingSnowball::BouncingSnowball(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/bouncing_snowball/bouncing_snowball.sprite"),
  m_x_speed(),
  m_turn_around_timer(),
  m_turn_around_counter()
{
  m_x_speed = BSNOWBALL_WALKSPEED;
  parse_type(reader);
}

BouncingSnowball::BouncingSnowball(const Vector& pos, Direction d, float x_vel) :
  BadGuy(pos, d, "images/creatures/bouncing_snowball/bouncing_snowball.sprite"),
  m_x_speed(),
  m_turn_around_timer(),
  m_turn_around_counter()
{
  m_countMe = false;
  m_x_speed = x_vel;
}

void
BouncingSnowball::initialize()
{
  m_physic.set_velocity_x(m_dir == Direction::LEFT ? -m_x_speed : m_x_speed);
  set_action(m_dir);
}

void
BouncingSnowball::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);
  if (m_frozen)
    return;

  if ((m_sprite->get_action() == "left-up" || m_sprite->get_action() == "right-up") && m_sprite->animation_done())
  {
    set_action(m_dir);
  }
  Rectf lookbelow = get_bbox();
  lookbelow.set_bottom(get_bbox().get_bottom() + 48);
  lookbelow.set_left(get_bbox().get_left() + 10);
  lookbelow.set_right(get_bbox().get_right() - 10);
  lookbelow.set_top(get_bbox().get_top() + 31);
  bool groundBelow = !Sector::get().is_free_of_statics(lookbelow);
  if (groundBelow && (m_physic.get_velocity_y() >= 64.0f))
  {
    set_action(m_dir, "down");
  }
  if (!groundBelow && (m_sprite->get_action() == "left-down" || m_sprite->get_action() == "right-down"))
  {
    set_action(m_dir);
  }

  // Left-right faux collision

  Rectf side_look_box = get_bbox().grown(-1.f);
  side_look_box.set_left(get_bbox().get_left() + (m_dir == Direction::LEFT ? -1.f : 1.f));
  side_look_box.set_right(get_bbox().get_right() + (m_dir == Direction::LEFT ? -1.f : 1.f));
  if (!Sector::get().is_free_of_statics(side_look_box, nullptr, true))
  {
    m_dir = m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
    set_action(m_dir);
    m_physic.set_velocity_x(-m_physic.get_velocity_x());
  }
}

GameObjectTypes
BouncingSnowball::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "fatbat", _("Fatbat") }
  };
}

std::string
BouncingSnowball::get_default_sprite_name() const
{
  switch (m_type)
  {
    case FATBAT:
      return "images/creatures/fatbat/fatbat.sprite";
    default:
      return m_default_sprite_name;
  }
}

bool
BouncingSnowball::is_freezable() const
{
  return m_type == FATBAT;
}

bool
BouncingSnowball::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return BadGuy::collision_squished(object);

  set_action("squished", m_dir);
  kill_squished(object);
  return true;
}

void
BouncingSnowball::collision_solid(const CollisionHit& hit)
{
  if (m_sprite->get_action() == "squished")
    return;

  if (m_frozen)
  {
    BadGuy::collision_solid(hit);
    return;
  }

  if (hit.bottom) {
    if (get_state() == STATE_ACTIVE) {
      float bounce_speed = -m_physic.get_velocity_y()*0.8f;
      m_physic.set_velocity_y(std::min(JUMPSPEED, bounce_speed));
      set_action(m_dir, "up", /* loops = */ 1);
    } else {
      m_physic.set_velocity_y(0);
    }
  } else if (hit.top) {
    m_physic.set_velocity_y(0);
  }

  //Left/right collisions handled in update because otherwise we would get weird wall-hugging behavior.

}

HitResponse
BouncingSnowball::collision_badguy(BadGuy& , const CollisionHit& hit)
{
  if (!m_frozen && ((hit.left && (m_dir == Direction::LEFT)) || (hit.right && (m_dir == Direction::RIGHT))))
    turn_around();
  else
    collision_solid(hit);

  return CONTINUE;
}

void
BouncingSnowball::turn_around()
{
  m_dir = m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
  set_action(m_dir);
  m_physic.set_velocity_x(-m_physic.get_velocity_x());
  m_physic.set_acceleration_x(-m_physic.get_acceleration_x());

  // if we get dizzy, we fall off the screen
  if (m_turn_around_timer.started()) {
    if (m_turn_around_counter++ > 10) kill_fall();
  } else {
    m_turn_around_timer.start(1.0f);
    m_turn_around_counter = 0;
  }
}

void
BouncingSnowball::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
  initialize();
}

void
BouncingSnowball::after_editor_set()
{
  BadGuy::after_editor_set();
  set_action(m_dir);
}

/* EOF */
