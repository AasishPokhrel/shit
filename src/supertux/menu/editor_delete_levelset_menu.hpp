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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_DELETE_LEVELSET_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_DELETE_LEVELSET_MENU_HPP

#include "gui/menu.hpp"
#include "gui/menu_item.hpp"

class EditorLevelsetSelectMenu;

class EditorDeleteLevelsetMenu final : public Menu
{
public:
  EditorDeleteLevelsetMenu(EditorLevelsetSelectMenu* editor_levelset_select_menu);

  virtual void refresh() override;
  virtual void menu_action(MenuItem& item) override;

private:
  EditorLevelsetSelectMenu* m_editor_levelset_select_menu;
  std::vector<std::string> m_world_names;

private:
  EditorDeleteLevelsetMenu(const EditorDeleteLevelsetMenu&) = delete;
  EditorDeleteLevelsetMenu& operator=(const EditorDeleteLevelsetMenu&) = delete;
};

#endif
/* EOF */
