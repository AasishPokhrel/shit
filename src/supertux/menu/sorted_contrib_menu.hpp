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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_SORTED_CONTRIB_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_SORTED_CONTRIB_MENU_HPP
#include "gui/menu.hpp"
#include "supertux/world.hpp"
class SortedContribMenu : public Menu
{
public:
  SortedContribMenu(std::vector<std::unique_ptr<World>>& worlds, const std::string& contrib_type, const std::string& title, const std::string& empty_message);
  void menu_action(MenuItem& item) override;
private:
  std::vector<std::string> m_world_folders;
private:
  SortedContribMenu(const SortedContribMenu&) = delete;
  SortedContribMenu& operator=(const SortedContribMenu&) = delete;
};
#endif
/* EOF */
