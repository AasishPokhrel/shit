//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_EDITOR_MARKER_OBJECT_HPP
#define HEADER_SUPERTUX_EDITOR_MARKER_OBJECT_HPP

#include "supertux/moving_object.hpp"

#include "video/layer.hpp"

class DrawingContext;

class MarkerObject : public MovingObject
{
public:
  MarkerObject(const Vector& pos);
  MarkerObject();
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(MarkerObject)); }

  virtual void update(float dt_sec) override {}
  virtual void draw(DrawingContext& context) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override { return FORCE_MOVE; }

  virtual Vector get_point_vector() const = 0;
  virtual Vector get_offset() const = 0;
  virtual bool hide_if_no_offset() const { return false; }

  virtual bool is_saveable() const override { return false; }
  bool track_state() const override { return false; }

  virtual int get_layer() const override { return LAYER_GUI - 20; }

  virtual void save_state() override = 0;
  virtual void check_state() override = 0;

private:
  MarkerObject(const MarkerObject&) = delete;
  MarkerObject& operator=(const MarkerObject&) = delete;
};

#endif

/* EOF */
