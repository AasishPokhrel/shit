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

#ifndef HEADER_SUPERTUX_EDITOR_RESIZE_MARKER_HPP
#define HEADER_SUPERTUX_EDITOR_RESIZE_MARKER_HPP

#include "editor/marker_object.hpp"

class MovingObject;

class ResizeMarker final : public MarkerObject
{
public:
  enum class Side {
    NONE,
    LEFT_UP,
    RIGHT_DOWN
  };

public:
  ResizeMarker(MovingObject* obj, Side vert, Side horz);
  virtual GameObjectClasses get_class_types() const override { return MarkerObject::get_class_types().add(typeid(ResizeMarker)); }

  void move_to(const Vector& pos) override;
  Vector get_point_vector() const override;
  Vector get_offset() const override;
  bool has_settings() const override { return false; }
  void editor_update() override;

  void save_state() override;
  void check_state() override;

private:
  void refresh_pos();

private:
  MovingObject* m_object;
  Rectf* m_rect;
  Side m_vert;
  Side m_horz;

private:
  ResizeMarker(const ResizeMarker&) = delete;
  ResizeMarker& operator=(const ResizeMarker&) = delete;
};

#endif

/* EOF */
