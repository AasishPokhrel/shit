//  SuperTux
//  Copyright (C) 2024 Daniel Ward <weluvgoatz@gmail.com>
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

#include "object/sticky_object.hpp"

#include "badguy/badguy.hpp"
#include "badguy/crusher.hpp"
#include "object/fallblock.hpp"
#include "object/moving_sprite.hpp"
#include "object/platform.hpp"
#include "object/tilemap.hpp"

StickyObject::StickyObject(const Vector& pos, const std::string& sprite_name,
                           int layer, CollisionGroup collision_group) :
  MovingSprite(pos, sprite_name, layer, collision_group),
  m_sticky(),
  m_sticking(),
  m_displacement_from_owner()
{
}

StickyObject::StickyObject(const ReaderMapping& reader, const std::string& sprite_name, int layer, CollisionGroup collision_group) :
  MovingSprite(reader, sprite_name, layer, collision_group),
  m_sticky(),
  m_sticking(),
  m_displacement_from_owner()
{
}

void
StickyObject::update(float dt_sec)
{
  const Rectf large_overlap_box = get_bbox().grown(8.f);
  for (auto& tm : Sector::get().get_objects_by_type<TileMap>())
  {
    if (large_overlap_box.overlaps(tm.get_bbox()) && tm.is_solid() && glm::length(tm.get_movement(true)) > (1.f * dt_sec) &&
      !Sector::get().is_free_of_statics(large_overlap_box))
    {
      m_col.set_movement(tm.get_movement(true));
      if (!m_sticking)
      {
        m_displacement_from_owner = get_pos() - tm.get_bbox().p1();
        m_sticking = true;
      }
      m_col.set_pos(tm.get_bbox().p1() + m_displacement_from_owner);
      return;
    }
  }

  sticky_update<Crusher>();
  sticky_update<Platform>();
  sticky_update<FallBlock>();
}

ObjectSettings
StickyObject::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();
  result.add_bool(_("Sticky"), &m_sticky, "sticky", false);

  result.reorder({"sticky", "sprite", "z-pos", "x", "y" });

  return result;
}

void
StickyObject::move_for_owner(MovingObject& object)
{
  m_col.set_pos(object.get_pos() + m_displacement_from_owner);
}

StickyBadguy::StickyBadguy(const ReaderMapping& mapping, const std::string& sprite_name, Direction default_direction, int layer, CollisionGroup collision_group) :
  BadGuy(mapping, sprite_name, default_direction, layer),
  m_sticky(),
  m_sticking(),
  m_displacement_from_owner()
{
  set_group(collision_group);
}

StickyBadguy::StickyBadguy(const ReaderMapping& mapping, const std::string& sprite_name, int layer, CollisionGroup collision_group) :
  BadGuy(mapping, sprite_name, layer),
  m_sticky(),
  m_sticking(),
  m_displacement_from_owner()
{
  set_group(collision_group);
}

void
StickyBadguy::sticky_update(float dt_sec)
{
  const Rectf large_overlap_box = get_bbox().grown(8.f);
  for (auto& tm : Sector::get().get_objects_by_type<TileMap>())
  {
    if (large_overlap_box.overlaps(tm.get_bbox()) && tm.is_solid() && glm::length(tm.get_movement(true)) > (1.f * dt_sec) &&
      !Sector::get().is_free_of_statics(large_overlap_box))
    {
      m_col.set_movement(tm.get_movement(true));
      if (!m_sticking)
      {
        m_displacement_from_owner = get_pos() - tm.get_bbox().p1();
        m_sticking = true;
      }
      m_col.set_pos(tm.get_bbox().p1() + m_displacement_from_owner);
      return;
    }
  }

  sticky_update<Crusher>();
  sticky_update<Platform>();
  sticky_update<FallBlock>();
}

ObjectSettings
StickyBadguy::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();
  result.add_bool(_("Sticky"), &m_sticky, "sticky", false);

  result.reorder({ "sticky", "direction", "sprite", "z-pos", "x", "y" });

  return result;
}

void
StickyBadguy::move_for_owner(MovingObject& object)
{
  m_col.set_pos(object.get_pos() + m_displacement_from_owner);
}

/* EOF */
