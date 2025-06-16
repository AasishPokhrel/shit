//  SuperTux
//  Copyright (C) 2021 mrkubax10 <mrkubax10@onet.pl>
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

#ifndef HEADER_SUPERTUX_GUI_ITEM_STRING_ARRAY_HPP
#define HEADER_SUPERTUX_GUI_ITEM_STRING_ARRAY_HPP

#include "gui/menu_item.hpp"
#include <vector>

class ItemStringArray final : public MenuItem
{
public:
  ItemStringArray(const std::string& text, std::vector<std::string>& items, int id = -1);

  virtual void process_action(const MenuAction& action) override;

private:
  std::vector<std::string>& m_items;

private:
  ItemStringArray(const ItemStringArray&) = delete;
  ItemStringArray& operator=(const ItemStringArray&) = delete;
};

#endif
/* EOF */
