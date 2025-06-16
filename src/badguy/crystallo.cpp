//  SuperTux - Crystallo
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

#include "badguy/crystallo.hpp"

#include "badguy/rcrystallo.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

Crystallo::Crystallo(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/crystallo/crystallo.sprite", "left", "right"),
  m_radius()
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
  reader.get("radius", m_radius, 100.0f);
}

Crystallo::Crystallo(const Vector& pos, const Vector& start_pos, float vel_x, std::unique_ptr<Sprite> sprite,
                     Direction dir, float radius, const std::string& script) :
  WalkingBadguy(pos, dir, "images/creatures/crystallo/crystallo.sprite", "left", "right"),
  m_radius(radius)
{
  m_physic.set_velocity_x(vel_x);
  m_sprite = std::move(sprite);
  m_dead_script = script;
  m_start_position = start_pos;
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
}

ObjectSettings
Crystallo::get_settings()
{
  ObjectSettings result = WalkingBadguy::get_settings();

  result.add_float(_("Radius"), &m_radius, "radius", 100.0f);

  result.reorder({"radius", "direction", "x", "y"});

  return result;
}

void
Crystallo::active_update(float dt_sec)
{
  // Walking and turning properly.
  float targetwalk = m_dir == Direction::LEFT ? -80.f : 80.f;
  if (m_dir != Direction::LEFT && get_pos().x > (m_start_position.x + m_radius - 20.f))
    targetwalk = -80.f;
  if (m_dir != Direction::RIGHT && get_pos().x < (m_start_position.x - m_radius + 20.f))
    targetwalk = 80.f;
  set_action(std::abs(m_physic.get_velocity_x()) < 80.f ?
    m_dir == Direction::LEFT ? "slowdown-left" : "slowdown-right" :
    m_dir == Direction::LEFT ? "left" : "right", -1);
  WalkingBadguy::active_update(dt_sec, targetwalk, 2.f);
}

bool
Crystallo::collision_squished(MovingObject& object)
{
  set_action(m_dir == Direction::LEFT ? "shattered-left" : "shattered-right", /* loops = */ -1, ANCHOR_BOTTOM);
  kill_squished(object);
  m_physic.set_gravity_modifier(1.f);
  m_physic.set_velocity_x(0.0);
  m_physic.set_acceleration_x(0.0);
  return true;
}

bool
Crystallo::is_flammable() const
{
  return false;
}

void
Crystallo::on_flip(float height)
{
  WalkingBadguy::on_flip(height);

  Sector::get().add<RCrystallo>(get_pos(), m_start_position, get_velocity_x(),
                                std::move(m_sprite), m_dir, m_radius, m_dead_script);
  remove_me();
}

/* EOF */
