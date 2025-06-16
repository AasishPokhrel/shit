//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_CHEAT_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_CHEAT_MENU_HPP

#include "gui/menu.hpp"

#include <functional>

class PlayerStatus;

class WorldmapCheatMenu final : public Menu
{
private:
  enum MenuIDs {
    MNID_GROW,
    MNID_FIRE,
    MNID_ICE,
    MNID_AIR,
    MNID_EARTH,
    MNID_SHRINK,
    MNID_GHOST,
    MNID_FINISH_LEVEL,
    MNID_RESET_LEVEL,
    MNID_FINISH_WORLDMAP,
    MNID_RESET_WORLDMAP,
    MNID_MOVE_TO_LEVEL,
    MNID_MOVE_TO_MAIN
  };

public:
  WorldmapCheatMenu();

  void menu_action(MenuItem& item) override;

private:
  void do_cheat(PlayerStatus& status, std::function<void(int)> callback);

private:
  WorldmapCheatMenu(const WorldmapCheatMenu&) = delete;
  WorldmapCheatMenu& operator=(const WorldmapCheatMenu&) = delete;
};

class WorldmapLevelSelectMenu final : public Menu
{
public:
  WorldmapLevelSelectMenu();

  void menu_action(MenuItem& item) override;

private:
  WorldmapLevelSelectMenu(const WorldmapLevelSelectMenu&) = delete;
  WorldmapLevelSelectMenu& operator=(const WorldmapLevelSelectMenu&) = delete;
};

#endif

/* EOF */
