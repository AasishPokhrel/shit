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

#include "supertux/menu/editor_new_levelset_menu.hpp"

#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/menu/editor_level_select_menu.hpp"
#include "supertux/world.hpp"
#include "util/gettext.hpp"

EditorNewLevelsetMenu::EditorNewLevelsetMenu() :
  levelset_name(),
  levelset_desc()
{
  add_label(_("New World"));
  add_hl();

  add_textfield(_("Name"), &levelset_name);
  add_textfield(_("Description"), &levelset_desc);

  add_entry(1,_("OK"));

  add_hl();
  add_back(_("Back"));
}

void
EditorNewLevelsetMenu::menu_action(MenuItem& item)
{
  if (item.get_id() <= 0)
    return;

  if (levelset_name.empty())
  {
    Dialog::show_message(_("Please enter a name for this level subset."));
    return;
  }

  std::unique_ptr<World> new_world = World::create(levelset_name, levelset_desc);
  new_world->save();

  MenuManager::instance().pop_menu();
  MenuManager::instance().push_menu(std::make_unique<EditorLevelSelectMenu>(std::move(new_world)));
}

/* EOF */
