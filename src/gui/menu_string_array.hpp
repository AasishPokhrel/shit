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

#ifndef HEADER_SUPERTUX_GUI_MENU_STRING_ARRAY_HPP
#define HEADER_SUPERTUX_GUI_MENU_STRING_ARRAY_HPP

#include "gui/menu.hpp"

#include <vector>

class StringArrayMenu final : public Menu
{
public:
  StringArrayMenu(std::vector<std::string>& items);

  virtual void menu_action(MenuItem& item) override;

private:
  void reload();

private:
  std::vector<std::string>& m_array_items;
  std::string m_text;
  int m_selected_item;

private:
  StringArrayMenu(const StringArrayMenu&) = delete;
  StringArrayMenu& operator=(const StringArrayMenu&) = delete;
};

#endif
/* EOF */
