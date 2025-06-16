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

#include "gui/menu.hpp"

class EditorLevelSelectMenu;
class EditorLevelsetSelectMenu;
class Levelset;
class World;

class EditorDeleteLevelMenu final : public Menu
{
private:
  Levelset* m_levelset;

  std::vector<std::string> m_level_full_paths;
  std::vector<std::string> m_level_names;
  EditorLevelSelectMenu* m_level_select_menu;
  EditorLevelsetSelectMenu* m_levelset_select_menu;

public:
  EditorDeleteLevelMenu(World* world, Levelset* levelset,
                        EditorLevelSelectMenu* level_select_menu, EditorLevelsetSelectMenu* levelset_select_menu);

  virtual void refresh() override;
  virtual void menu_action(MenuItem& item) override;

private:
  EditorDeleteLevelMenu(const EditorDeleteLevelMenu&) = delete;
  EditorDeleteLevelMenu& operator=(const EditorDeleteLevelMenu&) = delete;
};

/* EOF */
