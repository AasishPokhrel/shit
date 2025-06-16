//  SuperTux - Smart Snowball
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

#include "badguy/smartball.hpp"

#include "sprite/sprite.hpp"

SmartBall::SmartBall(const ReaderMapping& reader)
  : WalkingBadguy(reader, "images/creatures/snowball/smart-snowball.sprite", "left", "right")
{
  parse_type(reader);

  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
}

GameObjectTypes
SmartBall::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "pumpkin", _("Pumpkin") }
  };
}

std::string
SmartBall::get_default_sprite_name() const
{
  switch (m_type)
  {
    case PUMPKIN:
      return "images/creatures/pumpkin/pumpkin.sprite";
    default:
      return m_default_sprite_name;
  }
}

bool
SmartBall::is_freezable() const
{
  return m_type == PUMPKIN;
}

bool
SmartBall::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return WalkingBadguy::collision_squished(object);

  set_action("squished", m_dir);
  kill_squished(object);
  return true;
}

/* EOF */
