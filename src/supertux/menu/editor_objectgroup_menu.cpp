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

#include "supertux/menu/editor_objectgroup_menu.hpp"

#include "editor/editor.hpp"
#include "editor/object_group.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/level.hpp"
#include "util/gettext.hpp"

EditorObjectgroupMenu::EditorObjectgroupMenu()
{
  bool worldmap = Editor::current()->get_level()->is_worldmap();

  add_label(_("Objects"));
  add_hl();

  int id = 0;
  for (auto& og : Editor::current()->get_objectgroups()) {
    if (worldmap == og.is_worldmap()) {
      add_entry(id, og.get_name());
    }
    id++;
  }

  add_hl();
  add_entry(-1,_("Cancel"));
}

EditorObjectgroupMenu::~EditorObjectgroupMenu()
{
  auto editor = Editor::current();
  if (editor == nullptr) {
    return;
  }
  editor->m_reactivate_request = true;
}

void
EditorObjectgroupMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0)
  {
    Editor::current()->select_objectgroup(item.get_id());
  }
  MenuManager::instance().clear_menu_stack();
}

/* EOF */
