//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_INFO_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_INFO_HPP

#include "editor/object_group.hpp"

#include <memory>

class ObjectInfo final
{
public:
  ObjectInfo();

  inline int get_num_worldmap_groups() const { return get_num_groups(true); }
  inline int get_num_level_groups() const { return get_num_groups(false); }

  int get_first_worldmap_group_index() const;

private:
  int get_num_groups(bool for_worldmap) const;

public:
  std::vector<ObjectGroup> m_groups;
  std::unique_ptr<ObjectGroup> m_layers_group;
  std::unique_ptr<ObjectGroup> m_worldmap_layers_group;

private:
  ObjectInfo(const ObjectInfo&) = delete;
  ObjectInfo& operator=(const ObjectInfo&) = delete;
};

#endif

/* EOF */
