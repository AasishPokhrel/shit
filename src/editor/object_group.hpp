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

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_GROUP_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_GROUP_HPP

#include <string>
#include <vector>

#include "editor/object_icon.hpp"

class ReaderMapping;

class ObjectGroup final
{
public:
  ObjectGroup();
  ObjectGroup(const ReaderMapping& reader);

  void add_icon(const std::string& object, const std::string& icon_path);

  inline const std::string& get_name() const { return m_name; }

  bool is_worldmap() const { return m_for_worldmap;  }

  inline const std::vector<ObjectIcon>& get_icons() const { return m_icons; }
  inline std::vector<ObjectIcon>& get_icons() { return m_icons; }

private:
  std::string m_name;
  std::vector<ObjectIcon> m_icons;
  bool m_for_worldmap;
};

#endif

/* EOF */
