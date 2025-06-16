//  SuperTux
//  Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
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

#include "badguy/captainsnowball.hpp"

#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"

namespace{
  static const float CAPTAIN_WALK_SPEED = 100;
  static const float BOARDING_SPEED = 200;
}

CaptainSnowball::CaptainSnowball(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/snowball/captain-snowball.sprite", "left", "right"),
  m_jumping(false)
{
  walk_speed = BOARDING_SPEED;
  set_ledge_behavior(LedgeBehavior::FALL);
}

bool
CaptainSnowball::might_climb(int width, int height) const
{
  // Make sure we check for at least a 1-pixel climb.
  assert(height > 0);

  float x1;
  float x2;
  float y1a = m_col.m_bbox.get_top() + 1;
  float y2a = m_col.m_bbox.get_bottom() - 1;
  float y1b = m_col.m_bbox.get_top() + 1 - static_cast<float>(height);
  float y2b = m_col.m_bbox.get_bottom() - 1 - static_cast<float>(height);
  if (m_dir == Direction::LEFT) {
    x1 = m_col.m_bbox.get_left() - static_cast<float>(width);
    x2 = m_col.m_bbox.get_left() - 1;
  } else {
    x1 = m_col.m_bbox.get_right() + 1;
    x2 = m_col.m_bbox.get_right() + static_cast<float>(width);
  }
  return ((!Sector::get().is_free_of_statics(Rectf(x1, y1a, x2, y2a))) &&
          (Sector::get().is_free_of_statics(Rectf(x1, y1b, x2, y2b))));
}

void
CaptainSnowball::active_update(float dt_sec)
{
  const bool will_climb = on_ground() && might_climb(8, 64);
  const bool will_fall = on_ground() && might_fall(16);
  if (will_climb || will_fall)
  {
    m_jumping = true;
    set_action("jump", m_dir, 1);
    m_physic.set_velocity_y(-400);
  }
  if (will_fall)
  {
    walk_speed = BOARDING_SPEED;
    m_physic.set_velocity_x(m_dir == Direction::LEFT ? -walk_speed : walk_speed);
  }
  WalkingBadguy::active_update(dt_sec);
}

void
CaptainSnowball::collision_solid(const CollisionHit& hit)
{
  if (m_jumping && get_state() != STATE_SQUISHED)
  {
    m_jumping = false;
    set_action(m_dir);
  }
  if (is_active() && (walk_speed == BOARDING_SPEED))
  {
    walk_speed = CAPTAIN_WALK_SPEED;
    m_physic.set_velocity_x(m_dir == Direction::LEFT ? -walk_speed : walk_speed);
  }
  WalkingBadguy::collision_solid(hit);
}

bool
CaptainSnowball::collision_squished(MovingObject& object)
{
  set_action("squished", m_dir);
  kill_squished(object);
  return true;
}

/* EOF */
