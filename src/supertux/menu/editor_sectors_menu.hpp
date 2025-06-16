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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_SECTORS_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_SECTORS_MENU_HPP

#include "gui/menu.hpp"

class EditorSectorsMenu final : public Menu
{
public:
  EditorSectorsMenu();
  ~EditorSectorsMenu() override;

  void menu_action(MenuItem& item) override;

private:
  void create_sector();
  void delete_sector();

private:
  EditorSectorsMenu(const EditorSectorsMenu&) = delete;
  EditorSectorsMenu& operator=(const EditorSectorsMenu&) = delete;
};

#endif

/* EOF */
