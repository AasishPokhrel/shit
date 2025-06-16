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

#include "editor/object_info.hpp"
#include "util/reader_mapping.hpp"
#include "util/reader_document.hpp"

ObjectInfo::ObjectInfo() :
  m_groups(),
  m_layers_group(),
  m_worldmap_layers_group()
{
  auto doc = ReaderDocument::from_file("images/engine/editor/objects.stoi");
  auto root = doc.get_root();

  if (root.get_name() != "supertux-objectinfo")
    throw std::runtime_error("'images/engine/editor/objects.stoi' is not a 'supertux-objectinfo' file.");

  auto reader = root.get_mapping();

  auto iter = reader.get_iter();
  while (iter.next())
  {
    const std::string& token = iter.get_key();
    if (token == "objectgroup")
      m_groups.push_back(ObjectGroup(iter.as_mapping()));
    else if (token == "layers")
      m_layers_group = std::make_unique<ObjectGroup>(iter.as_mapping());
    else if (token == "layers-worldmap")
      m_worldmap_layers_group = std::make_unique<ObjectGroup>(iter.as_mapping());
  }
}

int
ObjectInfo::get_first_worldmap_group_index() const
{
  int worldmap_group_index = 0;
  for (const auto& group : m_groups)
  {
    if (group.is_worldmap())
    {
      return worldmap_group_index;
    }
    worldmap_group_index++;
  }
  return -1;
}

int
ObjectInfo::get_num_groups(bool for_worldmap) const
{
  int num_groups = 0;
  for (const auto& group : m_groups)
  {
    if (group.is_worldmap() == for_worldmap)
    {
      num_groups++;
    }
  }
  return num_groups;
}

/* EOF */
