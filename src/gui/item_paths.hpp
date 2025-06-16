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

#ifndef HEADER_SUPERTUX_GUI_ITEM_PATHS_HPP
#define HEADER_SUPERTUX_GUI_ITEM_PATHS_HPP

#include "gui/menu_item.hpp"

class PathObject;

class ItemPaths final : public MenuItem
{
public:
  ItemPaths(const std::string& text, PathObject& target, const std::string& path_ref, int id = -1);

  /** Processes the menu action. */
  virtual void process_action(const MenuAction& action) override;

private:
  PathObject& m_target;
  std::string m_path_ref;

private:
  ItemPaths(const ItemPaths&) = delete;
  ItemPaths& operator=(const ItemPaths&) = delete;
};

#endif

/* EOF */
