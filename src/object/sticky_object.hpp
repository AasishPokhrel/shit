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

#ifndef HEADER_SUPERTUX_OBJECT_STICKY_OBJECT_HPP
#define HEADER_SUPERTUX_OBJECT_STICKY_OBJECT_HPP

#include "badguy/badguy.hpp"
#include "object/moving_sprite.hpp"

#include "supertux/sector.hpp"

/** This is a class for MovingSprites that can stick to the sides, top and bottom of MovingObjects,
    such as platforms, fallblock, tilemap, etc. */
class StickyObject : public MovingSprite
{
public:
  StickyObject(const Vector& pos, const std::string& sprite_name,
               int layer = LAYER_OBJECTS, CollisionGroup collision_group = COLGROUP_MOVING);
  StickyObject(const ReaderMapping& reader, const std::string& sprite_name,
               int layer = LAYER_OBJECTS, CollisionGroup collision_group = COLGROUP_MOVING);
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(StickyObject)); }

  virtual void update(float dt_sec) override;

  virtual ObjectSettings get_settings() override;

  virtual void move_for_owner(MovingObject& object);

  inline bool is_sticky() const { return m_sticky; }

private:
  template<class T>
  void sticky_update()
  {
    for (auto& obj : Sector::get().get_objects_by_type<T>())
    {
      if (m_col.m_bbox.grown(8.f).overlaps(obj.get_bbox()))
      {
        m_col.set_movement(obj.get_movement());
        if (!m_sticking)
        {
          m_displacement_from_owner = get_pos() - obj.get_pos();
          m_sticking = true;
        }
        move_for_owner(obj);
        return;
      }
    }
  }

protected:
  bool m_sticky; // determines if the object CAN stick, period. 
  bool m_sticking; // determines if the object has found something to stick to.
  Vector m_displacement_from_owner;

private:
  StickyObject(const StickyObject&) = delete;
  StickyObject& operator=(const StickyObject&) = delete;
};


/** This is a class for BadGuys that can stick to the sides, top and bottom of MovingObjects,
    such as platforms, fallblock, tilemap, etc. */
class StickyBadguy : public BadGuy
{
public:
  StickyBadguy(const ReaderMapping& reader, const std::string& sprite_name, Direction default_direction,
               int layer = LAYER_OBJECTS, CollisionGroup collision_group = COLGROUP_MOVING);
  StickyBadguy(const ReaderMapping& reader, const std::string& sprite_name,
               int layer = LAYER_OBJECTS, CollisionGroup collision_group = COLGROUP_MOVING);
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(StickyBadguy)); }

  virtual void sticky_update(float dt_sec);

  virtual ObjectSettings get_settings() override;

  virtual void move_for_owner(MovingObject& object);

  inline bool is_sticky() const { return m_sticky; }

private:
  template<class T>
  void sticky_update()
  {
    for (auto& obj : Sector::get().get_objects_by_type<T>())
    {
      if (m_col.m_bbox.grown(8.f).overlaps(obj.get_bbox()))
      {
        m_col.set_movement(obj.get_movement());
        if (!m_sticking)
        {
          m_displacement_from_owner = get_pos() - obj.get_pos();
          m_sticking = true;
        }
        move_for_owner(obj);
        return;
      }
    }
  }

protected:
  bool m_sticky; // determines if the object CAN stick, period. 
  bool m_sticking; // determines if the object has found something to stick to.
  Vector m_displacement_from_owner;

private:
  StickyBadguy(const StickyBadguy&) = delete;
  StickyBadguy& operator=(const StickyBadguy&) = delete;
};

#endif

/* EOF */
