//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_MENU_HPP

#include "gui/menu.hpp"

class EditorLevelMenu final : public Menu
{
private:
public:
  EditorLevelMenu();
  ~EditorLevelMenu() override;

  void menu_action(MenuItem& item) override;
  bool on_back_action() override;
  bool is_sensitive() const override {
    return true;
  }

private:
  std::string old_tileset;

private:
  EditorLevelMenu(const EditorLevelMenu&) = delete;
  EditorLevelMenu& operator=(const EditorLevelMenu&) = delete;
};

#endif

/* EOF */
