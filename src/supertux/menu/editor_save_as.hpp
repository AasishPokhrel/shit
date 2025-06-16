//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_SAVE_AS_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_SAVE_AS_HPP

#include "gui/menu.hpp"

class EditorSaveAs final : public Menu
{
private:
  enum MenuIDs {
    MNID_SAVE,
    MNID_CANCEL
  };

public:
  EditorSaveAs(bool do_switch_file);
  ~EditorSaveAs() override;

  void menu_action(MenuItem& item) override;

private:
  std::string m_filename;
  bool m_do_switch_file;

private:
  EditorSaveAs(const EditorSaveAs&) = delete;
  EditorSaveAs& operator=(const EditorSaveAs&) = delete;
};

#endif

/* EOF */
