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

#include "gui/item_script.hpp"

#include "gui/menu.hpp"
#include "gui/menu_manager.hpp"
#include "gui/menu_script.hpp"

ItemScript::ItemScript(const std::string& text, std::string* script_, int id) :
  MenuItem(text, id),
  script(script_)
{
}

void
ItemScript::process_action(const MenuAction& action) {
  if (action == MenuAction::HIT) {
    MenuManager::instance().push_menu(std::make_unique<ScriptMenu>(script));
  }
}

/* EOF */
