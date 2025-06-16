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

#ifndef HEADER_SUPERTUX_OBJECT_PATH_WALKER_HPP
#define HEADER_SUPERTUX_OBJECT_PATH_WALKER_HPP

#include <string.h>
#include <memory>

#include "math/sizef.hpp"
#include "object/path.hpp"
#include "util/uid.hpp"

template<typename T>
class ObjectOption;

/** A walker that travels along a path */
class PathWalker final
{
public:
  /** Helper class that allows to displace a handle on an object */
  class Handle
  {
  public:
    Handle() : m_scalar_pos(), m_pixel_offset() {}
    Vector get_pos(const Sizef& size, const Vector& pos) const;

  public:
    Vector m_scalar_pos; /**< The scale of the object the handle should be displaced to ((0,0) = top left, (1,1) = bottom right) */
    Vector m_pixel_offset; /**< The secondary displacement, in absolute size (pixels) */
  };

public:
  PathWalker(UID path_uid, bool running = true);
  ~PathWalker();

  /** advances the path walker on the path and returns its new position */
  void update(float dt_sec);

  /** current position of path walker */
  Vector get_pos(const Sizef& object_size, const Handle& handle) const;

  /** advance until at given node, then stop */
  void goto_node(int node_idx);

  /** teleport instantly to given node */
  void jump_to_node(int node_idx, bool instantaneous = false);

  /** start advancing automatically */
  void start_moving();

  /** stop advancing automatically */
  void stop_moving();

  /** returns true if PathWalker is currently moving */
  inline bool is_running() const { return m_running; }

private:
  void advance_node();
  void goback_node();
  Path* get_path() const;

public:
  UID m_path_uid;

  /** set to false to immediately stop advancing */
  bool m_running;

private:
  size_t m_current_node_nr;
  size_t m_next_node_nr;

  /** stop advancing automatically when this node is reached */
  int m_stop_at_node_nr;

  /** the position between the current node and the next node as
      fraction between 0 and 1 */
  float m_node_time;
  float m_node_mult;

  float m_walking_speed;

private:
  PathWalker(const PathWalker&) = delete;
  PathWalker& operator=(const PathWalker&) = delete;
};

#endif

/* EOF */
