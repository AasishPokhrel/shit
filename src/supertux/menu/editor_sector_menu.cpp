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

#include "supertux/menu/editor_sector_menu.hpp"

#include "gui/menu_item.hpp"
#include "editor/editor.hpp"
#include "supertux/level.hpp"
#include "supertux/sector.hpp"
#include "util/gettext.hpp"

#include <fmt/format.h>

EditorSectorMenu::EditorSectorMenu() :
  sector(Editor::current()->get_sector()),
  original_name(sector->get_name()),
  size(sector->get_editor_size()),
  new_size(size),
  offset(0, 0)
{ 
  add_label(fmt::format(fmt::runtime(_("Sector {}")), sector->get_name()));
  add_hl();
  add_textfield(_("Name"), &sector->m_name);
  add_script(_("Initialization script"), &sector->m_init_script);
  add_floatfield(_("Gravity"), &sector->m_gravity);

  add_hl();
  add_intfield(_("Width"), &(new_size.width));
  add_intfield(_("Height"), &(new_size.height));
  add_intfield(_("Resize offset X"), &(offset.width));
  add_intfield(_("Resize offset Y"), &(offset.height));
  add_entry(MNID_RESIZESECTOR, _("Resize"));

  add_hl();
  add_back(_("OK"));
}

EditorSectorMenu::~EditorSectorMenu()
{
  auto editor = Editor::current();
  if (editor == nullptr) {
    return;
  }
  // Makes sure that the name of the sector isn't already used.
  auto level = editor->get_level();
  bool is_sector = false;
  for (auto const& sector_ : level->m_sectors) {
    if (sector_->get_name() == sector->get_name()) {
      if (is_sector) {
        // Puts the name that was there before when the name is already used.
        sector->set_name(original_name);
        break;
      } else {
        is_sector = true;
      }
    }
  }
}

void
EditorSectorMenu::menu_action(MenuItem& item)
{
  switch (item.get_id()) {
    case MNID_RESIZESECTOR:
      if (new_size.is_valid()) {
        sector->resize(size, new_size, offset);
        size = new_size;
      }
      break;
  }
}

/* EOF */
