//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_COLLISION_COLLISION_OBJECT_HPP
#define HEADER_SUPERTUX_COLLISION_COLLISION_OBJECT_HPP

#include <stdint.h>
#include <memory>
#include <unordered_set>

#include "collision/collision_group.hpp"
#include "collision/collision_hit.hpp"
#include "math/rectf.hpp"

class CollisionGroundMovementManager;
class MovingObject;

class CollisionObject
{
  friend class CollisionSystem;

public:
  CollisionObject(CollisionGroup group, MovingObject& parent);

  /** this function is called when the object collided with something solid */
  void collision_solid(const CollisionHit& hit);

  /** when 2 objects collided, we will first call the
      pre_collision_check functions of both objects that can decide on
      how to react to the collision. */
  bool collides(CollisionObject& other, const CollisionHit& hit) const;

  /** this function is called when the object collided with any other object */
  HitResponse collision(CollisionObject& other, const CollisionHit& hit);

  /** called when tiles with special attributes have been touched */
  void collision_tile(uint32_t tile_attributes);

  /** called when this object, if (moving) static, has collided on its top with a moving object */
  void collision_moving_object_bottom(CollisionObject& other);

  void notify_object_removal(CollisionObject* other);

  inline void set_ground_movement_manager(const std::shared_ptr<CollisionGroundMovementManager>& movement_manager)
  {
    m_ground_movement_manager = movement_manager;
  }

  void clear_bottom_collision_list();

  inline bool is_unisolid() const { return m_unisolid; }
  inline void set_unisolid(bool unisolid) { m_unisolid = unisolid; }

  /** returns the bounding box of the Object */
  inline const Rectf& get_bbox() const
  {
    return m_bbox;
  }

  inline void set_movement(const Vector& movement)
  {
    m_movement = movement;
  }

  void propagate_movement(const Vector& movement);

  inline const Vector& get_movement() const
  {
    return m_movement;
  }

  /** places the moving object at a specific position. Be careful when
      using this function. There are no collision detection checks
      performed here so bad things could happen. */
  void set_pos(const Vector& pos)
  {
    m_dest.move(pos - get_pos());
    m_bbox.set_pos(pos);
  }

  inline Vector get_pos() const
  {
    return m_bbox.p1();
  }

  inline Vector get_pressure() const
  {
    return m_pressure;
  }

  /** moves entire object to a specific position, including all
      points those the object has, exactly like the object has
      spawned in that given pos instead.*/
  inline void move_to(const Vector& pos)
  {
    set_pos(pos);
  }

  /** sets the moving object's bbox to a specific width. Be careful
      when using this function. There are no collision detection
      checks performed here so bad things could happen. */
  void set_width(float w)
  {
    m_dest.set_width(w);
    m_bbox.set_width(w);
  }

  /** sets the moving object's bbox to a specific size. Be careful
      when using this function. There are no collision detection
      checks performed here so bad things could happen. */
  void set_size(float w, float h)
  {
    m_dest.set_size(w, h);
    m_bbox.set_size(w, h);
  }

  inline CollisionGroup get_group() const
  {
    return m_group;
  }

  bool is_valid() const;

  inline MovingObject& get_parent() { return m_parent; }

private:
  MovingObject& m_parent;

public:
  /** The bounding box of the object (as used for collision detection,
      this isn't necessarily the bounding box for graphics) */
  Rectf m_bbox;

  /** The collision group */
  CollisionGroup m_group;

private:
  /** The movement that will happen till next frame */
  Vector m_movement;

  /** This is only here for internal collision detection use (don't touch this
      from outside collision detection code)

      This field holds the currently anticipated destination of the object
      during collision detection */
  Rectf m_dest;

  /** Determines whether the object is unisolid.

      Only bottom constraints to the top of the bounding box would be applied for objects,
      colliding with unisolid objects. */
  bool m_unisolid;

  /**
   * Contains the current pressure on this object
   */
  Vector m_pressure;

  /** Objects that were touching the top of this object at the last frame,
      if this object was static or moving static. */
  std::unordered_set<CollisionObject*> m_objects_hit_bottom;

  std::shared_ptr<CollisionGroundMovementManager> m_ground_movement_manager;

private:
  CollisionObject(const CollisionObject&) = delete;
  CollisionObject& operator=(const CollisionObject&) = delete;
};

#endif

/* EOF */
