//  SuperTux -- List menu
//  Copyright (C) 2021 Rami <rami.slicer@gmail.com>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_GUI_MENU_LIST_HPP
#define HEADER_SUPERTUX_GUI_MENU_LIST_HPP

#include "gui/menu.hpp"

#include <functional>

class ListMenu final : public Menu
{
public:
  ListMenu(const std::vector<std::string>& entries, std::string* selected, Menu* parent,
           const std::function<std::string (const std::string&)>& text_processor = {});

  void menu_action(MenuItem& item) override;

private:
  const std::vector<std::string> m_entries;
  std::string* m_selected;
  Menu* m_parent;

private:
  ListMenu(const ListMenu&) = delete;
  ListMenu& operator=(const ListMenu&) = delete;
};

#endif

/* EOF */
