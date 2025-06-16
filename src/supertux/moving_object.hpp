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

#ifndef HEADER_SUPERTUX_SUPERTUX_MOVING_OBJECT_HPP
#define HEADER_SUPERTUX_SUPERTUX_MOVING_OBJECT_HPP

#include "supertux/game_object.hpp"

#include "collision/collision_hit.hpp"
#include "collision/collision_object.hpp"
#include "math/rectf.hpp"

class Dispenser;
class Sector;

/**
 * @scripting
 * @summary Base class for all dynamic/moving game objects. This class
            contains things for handling the bounding boxes and collision
            feedback.
 */
class MovingObject : public GameObject
{
  friend class ResizeMarker;
  friend class Sector;
  friend class CollisionSystem;

public:
  static void register_class(ssq::VM& vm);

public:
  MovingObject();
  MovingObject(const ReaderMapping& reader);
  ~MovingObject() override;
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(MovingObject)); }

  virtual void collision_solid(const CollisionHit& /*hit*/)
  {
  }

  virtual bool collides(MovingObject& /*other*/, const CollisionHit& /*hit*/) const
  {
    return true;
  }

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) = 0;

  virtual void collision_tile(uint32_t /*tile_attributes*/)
  {
  }

  virtual void set_pos(const Vector& pos)
  {
    m_col.set_pos(pos);
  }

  virtual void move_to(const Vector& pos)
  {
    m_col.move_to(pos);
  }
  virtual void move(const Vector& dist)
  {
    m_col.m_bbox.move(dist);
  }

  Vector get_pos() const
  {
    return m_col.m_bbox.p1();
  }

  const Rectf& get_bbox() const
  {
    return m_col.m_bbox;
  }

  const Vector& get_movement() const
  {
    return m_col.get_movement();
  }

  CollisionGroup get_group() const
  {
    return m_col.m_group;
  }

  CollisionObject* get_collision_object() {
    return &m_col;
  }

  const CollisionObject* get_collision_object() const {
    return &m_col;
  }

  void set_parent_dispenser(Dispenser* dispenser);
  inline Dispenser* get_parent_dispenser() const { return m_parent_dispenser; }

  static std::string class_name() { return "moving-object"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "MovingObject"; }
  virtual ObjectSettings get_settings() override;

  virtual void editor_select() override;

  virtual void on_flip(float height) override;

  virtual int get_layer() const = 0;

  /**
   * @scripting
   * @description Returns the object's X coordinate.
   */
  inline float get_x() const { return m_col.m_bbox.get_left(); }
  /**
   * @scripting
   * @description Returns the object's Y coordinate.
   */
  inline float get_y() const { return m_col.m_bbox.get_top(); }
  /**
   * @scripting
   * @description Sets the position of the object.
   * @param float $x
   * @param float $y
   */
  inline void set_pos(float x, float y) { set_pos(Vector(x, y)); }
  /**
   * @scripting
   * @description Moves the object by ""x"" units to the right and ""y"" down, relative to its current position.
   * @param float $x
   * @param float $y
   */
  inline void move(float x, float y) { move(Vector(x, y)); }

  /**
   * @scripting
   * @description Returns the object's hitbox width.
   */
  inline float get_width() const { return m_col.m_bbox.get_width(); }
  /**
   * @scripting
   * @description Returns the object's hitbox height.
   */
  inline float get_height() const { return m_col.m_bbox.get_height(); }

protected:
  void set_group(CollisionGroup group)
  {
    m_col.m_group = group;
  }

protected:
  CollisionObject m_col;

  Dispenser* m_parent_dispenser;

private:
  MovingObject(const MovingObject&) = delete;
  MovingObject& operator=(const MovingObject&) = delete;
};

#endif

/* EOF */
