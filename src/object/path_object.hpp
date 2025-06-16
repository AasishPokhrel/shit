//  SuperTux
//  Copyright (C) 2018 Tobias Markus <tobbi@supertux.org>
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


#ifndef HEADER_SUPERTUX_OBJECT_PATH_OBJECT_HPP
#define HEADER_SUPERTUX_OBJECT_PATH_OBJECT_HPP

#include <memory>

#include "object/path.hpp"
#include "object/path_gameobject.hpp"
#include "object/path_walker.hpp"
#include "util/uid.hpp"

namespace ssq {
class Class;
} // namespace ssq

/**
 * @scripting
 * @summary A base class for all objects that contain, or make use of a path.
 */
class PathObject
{
protected:
  static void register_members(ssq::Class& cls);

public:
  PathObject();
  virtual ~PathObject();

  /** For compatibilty reasons this needs to get the GameObjects
      main mapping, not the (path ...) mapping */
  void init_path(const ReaderMapping& mapping, bool running_default);
  void init_path_pos(const Vector& pos, bool running = false);

  PathGameObject* get_path_gameobject() const;
  Path* get_path() const;
  inline PathWalker* get_walker() const { return m_walker.get(); }

  bool has_valid_path() const { return get_walker() && get_path() && get_path()->is_valid(); }

  void editor_clone_path(PathGameObject* path_object);

  std::string get_path_ref() const;
  void editor_set_path_by_ref(const std::string& new_ref);

  virtual void on_path_resolved() {}

  /**
   * @scripting
   * @description Moves the path object until at given node, then stops.
   * @param int $node_idx
   */
  void goto_node(int node_idx);
  /**
   * @scripting
   * @description Jumps instantly to the given node.
   * @param int $node_idx
   */
  void set_node(int node_idx);
  /**
   * @scripting
   * @description Starts moving the path object automatically.
   */
  void start_moving();
  /**
   * @scripting
   * @description Stops moving the path object.
   */
  void stop_moving();

protected:
  void save_state() const;
  void check_state() const;

  void on_flip();

protected:
  PathWalker::Handle m_path_handle;

private:
  UID m_path_uid;
  std::unique_ptr<PathWalker> m_walker;

private:
  PathObject(const PathObject&) = delete;
  PathObject& operator=(const PathObject&) = delete;
};

#endif

/* EOF */
