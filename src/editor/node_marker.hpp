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

#ifndef HEADER_SUPERTUX_EDITOR_NODE_MARKER_HPP
#define HEADER_SUPERTUX_EDITOR_NODE_MARKER_HPP

#include <editor/bezier_marker.hpp>
#include "editor/marker_object.hpp"
#include "object/path.hpp"

class NodeMarker final : public MarkerObject
{
public:
  NodeMarker(std::vector<Path::Node>::iterator node_iterator, size_t id_, UID before, UID after);
  virtual GameObjectClasses get_class_types() const override { return MarkerObject::get_class_types().add(typeid(NodeMarker)); }

  virtual void move_to(const Vector& pos) override;
  virtual void editor_delete() override;
  virtual Vector get_point_vector() const override;
  virtual Vector get_offset() const override;
  virtual bool has_settings() const override { return true; }
  virtual ObjectSettings get_settings() override;
  virtual void editor_update() override;
  virtual void remove_me() override;

  void update_iterator();
  void update_node_times();

  /** Moves the bezier marker that ISN'T @c marker to the given position. */
  /** Can't make the by reference because of overlay_widget.cpp */
  void move_other_marker(UID marker, Vector position);

  void save_state() override;
  void check_state() override;

private:
  Path* m_path;
  std::vector<Path::Node>::iterator prev_node();
  std::vector<Path::Node>::const_iterator next_node() const;
  void update_node_time(std::vector<Path::Node>::iterator current, std::vector<Path::Node>::const_iterator next);

  UID m_bezier_before;
  UID m_bezier_after;

public:
  std::vector<Path::Node>::iterator m_node;

private:
  size_t m_id;

private:
  NodeMarker(const NodeMarker&) = delete;
  NodeMarker& operator=(const NodeMarker&) = delete;
};

#endif

/* EOF */
