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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_SELECT_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_SELECT_MENU_HPP

#include "gui/menu.hpp"

class EditorLevelsetSelectMenu;
class Levelset;
class World;

class EditorLevelSelectMenu final : public Menu
{
public:
  EditorLevelSelectMenu();
  EditorLevelSelectMenu(std::unique_ptr<World> world, EditorLevelsetSelectMenu* levelset_select_menu = nullptr);
  ~EditorLevelSelectMenu() override;

  void menu_action(MenuItem& item) override;

  void open_level(const std::string& filename);

  void reload_menu();

private:
  void initialize();
  void create_level();
  void create_worldmap();
  void create_item(bool worldmap);

  World* get_world() const;

private:
  std::unique_ptr<World> m_world; // Contains a provided world, not associated with the editor
  std::unique_ptr<Levelset> m_levelset;
  EditorLevelsetSelectMenu* m_levelset_select_menu;

private:
  EditorLevelSelectMenu(const EditorLevelSelectMenu&) = delete;
  EditorLevelSelectMenu& operator=(const EditorLevelSelectMenu&) = delete;
};

#endif

/* EOF */
