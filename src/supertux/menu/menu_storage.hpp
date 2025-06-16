//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_MENU_STORAGE_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_MENU_STORAGE_HPP

#include <memory>

class Menu;

class MenuStorage final
{
private:
  static MenuStorage* s_instance;
public:
  static MenuStorage& instance();

public:
  enum MenuId {
    NO_MENU,
    MAIN_MENU,
    OPTIONS_MENU,
    INGAME_OPTIONS_MENU,
    PROFILE_MENU,
    WORLDSET_MENU,
    CONTRIB_MENU,
    CONTRIB_WORLD_MENU,
    ADDON_MENU,
    LANGPACK_MENU,
    LANGPACK_AUTO_UPDATE_MENU,
    LANGUAGE_MENU,
    KEYBOARD_MENU,
    JOYSTICK_MENU,
    VIDEO_SYSTEM_MENU,
    WORLDMAP_MENU,
    WORLDMAP_CHEAT_MENU,
    WORLDMAP_LEVEL_SELECT_MENU,
    GAME_MENU,
    CHEAT_MENU,
    DEBUG_MENU,
    EDITOR_LEVELSET_SELECT_MENU,
    EDITOR_NEW_LEVELSET_MENU,
    EDITOR_LEVEL_SELECT_MENU,
    EDITOR_MENU,
    EDITOR_TILEGROUP_MENU,
    EDITOR_OBJECTGROUP_MENU,
    EDITOR_SECTORS_MENU,
    EDITOR_SECTOR_MENU,
    EDITOR_LEVEL_MENU,
    EDITOR_CONVERTERS_MENU,
    PARTICLE_EDITOR_MENU,
    PARTICLE_EDITOR_SAVE_AS,
    PARTICLE_EDITOR_OPEN,
    INTEGRATIONS_MENU,
    ASSET_MENU,
    CUSTOM_MENU_MENU,
    MULTIPLAYER_MENU,
    MULTIPLAYER_PLAYERS_MENU
  };

public:
  MenuStorage();
  ~MenuStorage();

  std::unique_ptr<Menu> create(MenuId menu_id);

private:
  MenuStorage(const MenuStorage&) = delete;
  MenuStorage& operator=(const MenuStorage&) = delete;
};

#endif

/* EOF */
