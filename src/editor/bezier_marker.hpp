//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_EDITOR_BEZIER_MARKER_HPP
#define HEADER_SUPERTUX_EDITOR_BEZIER_MARKER_HPP

#include "editor/marker_object.hpp"
#include "object/path.hpp"

class NodeMarker;

class BezierMarker final : public MarkerObject
{
public:
  BezierMarker(Path::Node* node, Vector* bezier_pos);
  virtual GameObjectClasses get_class_types() const override { return MarkerObject::get_class_types().add(typeid(BezierMarker)); }

  virtual void move_to(const Vector& pos) override;
  virtual Vector get_point_vector() const override;
  virtual Vector get_offset() const override;
  virtual bool hide_if_no_offset() const override { return true; }
  virtual bool has_settings() const override { return false; }
  virtual void editor_update() override;

  void update_iterator(Path::Node* it, Vector* bezier_pos);

  inline void set_parent(UID uid) { m_parent = uid; }
  NodeMarker* get_parent() const;

  void save_state() override;
  void check_state() override;

private:
  Path::Node* m_node;
  Vector* m_pos;
  Vector m_offset;
  UID m_parent;

private:
  BezierMarker(const BezierMarker&) = delete;
  BezierMarker& operator=(const BezierMarker&) = delete;
};

#endif

/* EOF */
