//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_CHEAT_APPLY_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_CHEAT_APPLY_MENU_HPP

#include "gui/menu.hpp"

class Player;

class WorldmapCheatApplyMenu final : public Menu
{
public:
  WorldmapCheatApplyMenu(int num_players, std::function<void(int)> callback);

  void menu_action(MenuItem& item) override;

private:
  int m_num_players;
  std::function<void(int)> m_callback;

private:
  WorldmapCheatApplyMenu(const WorldmapCheatApplyMenu&) = delete;
  WorldmapCheatApplyMenu& operator=(const WorldmapCheatApplyMenu&) = delete;
};

#endif

/* EOF */
