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

#ifndef HEADER_SUPERTUX_GUI_MENU_SCRIPT_HPP
#define HEADER_SUPERTUX_GUI_MENU_SCRIPT_HPP

#include "gui/menu.hpp"

class ItemScriptLine;

class ScriptMenu final : public Menu
{
public:
  ScriptMenu(std::string* script_);
  ~ScriptMenu() override;

  void menu_action(MenuItem& item) override;

  void remove_line();
  ItemScriptLine* add_line();

protected:
  bool is_sensitive() const override;

private:
  std::string* base_script;
  std::vector<std::unique_ptr<std::string> > script_strings;

  void push_string(const std::string& new_line);

private:
  ScriptMenu(const ScriptMenu&) = delete;
  ScriptMenu& operator=(const ScriptMenu&) = delete;
};

#endif

/* EOF */
