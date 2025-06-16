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

#include "editor/object_group.hpp"

#include "util/reader_mapping.hpp"

ObjectGroup::ObjectGroup() :
  m_name(),
  m_icons(),
  m_for_worldmap(false)
{
  m_icons.clear();
}

ObjectGroup::ObjectGroup(const ReaderMapping& reader) :
  m_name(),
  m_icons(),
  m_for_worldmap(false)
{
  m_icons.clear();

  reader.get("name", m_name);
  reader.get("worldmap", m_for_worldmap);

  auto iter = reader.get_iter();
  while (iter.next()) {
    const std::string& token = iter.get_key();
    if (token == "object") {
      m_icons.push_back( ObjectIcon( iter.as_mapping() ) );
    }
  }
}

void
ObjectGroup::add_icon(const std::string& object, const std::string& icon_path) {
  ObjectIcon new_icon(object, icon_path);
  m_icons.push_back(new_icon);
}

/* EOF */
