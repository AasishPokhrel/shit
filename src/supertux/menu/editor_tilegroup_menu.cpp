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

#include "supertux/menu/editor_tilegroup_menu.hpp"

#include "editor/editor.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "util/gettext.hpp"

EditorTilegroupMenu::EditorTilegroupMenu()
{
  add_label(_("Tiles"));
  add_hl();

  int id = 0;
  for (auto& tg : Editor::current()->get_tilegroups()) {
    add_entry(id, _(tg.name));
    id++;
  }

  add_hl();
  add_entry(-1,_("Cancel"));
}

EditorTilegroupMenu::~EditorTilegroupMenu()
{
  auto editor = Editor::current();
  if (editor == nullptr) {
    return;
  }
  editor->m_reactivate_request = true;
}

void
EditorTilegroupMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0)
  {
    Editor::current()->select_tilegroup(item.get_id());
  }
  MenuManager::instance().clear_menu_stack();
}

/* EOF */
