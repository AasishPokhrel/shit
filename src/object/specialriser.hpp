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

#ifndef HEADER_SUPERTUX_OBJECT_SPECIALRISER_HPP
#define HEADER_SUPERTUX_OBJECT_SPECIALRISER_HPP

#include "supertux/moving_object.hpp"

/**
 * special object that contains another object and slowly rises it out of a
 * bonus block.
 */
class SpecialRiser final : public MovingObject
{
public:
  SpecialRiser(const Vector& pos, std::unique_ptr<MovingObject> child, bool is_solid = false);
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(SpecialRiser)); }
  virtual bool is_saveable() const override {
    return false;
  }

  HitResponse collision(MovingObject& other, const CollisionHit& hit) override {
    return FORCE_MOVE;
  }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual int get_layer() const override { return m_child ? m_child->get_layer() : INT_MIN; }

private:
  Vector m_start_pos; 
  float m_offset;
  std::unique_ptr<MovingObject> m_child;

private:
  SpecialRiser(const SpecialRiser&) = delete;
  SpecialRiser& operator=(const SpecialRiser&) = delete;
};

#endif

/* EOF */
