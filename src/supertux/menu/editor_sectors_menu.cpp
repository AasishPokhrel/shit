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

#include "supertux/menu/editor_sectors_menu.hpp"

#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/level.hpp"
#include "supertux/sector.hpp"
#include "supertux/sector_parser.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

EditorSectorsMenu::EditorSectorsMenu()
{
  add_label(_("Choose Sector"));
  add_hl();

  int id = 0;
  for (const auto& sector : Editor::current()->get_level()->m_sectors) {
    add_entry(id, sector->get_name());
    id++;
  }

  add_hl();
  add_submenu(_("Sector Settings"), MenuStorage::EDITOR_SECTOR_MENU);
  add_entry(-2,_("Create Sector"));
  add_entry(-3,_("Delete Sector"));
  add_hl();
  add_entry(-4,_("Cancel"));
}

EditorSectorsMenu::~EditorSectorsMenu()
{
  auto editor = Editor::current();
  if (editor == nullptr) {
    return;
  }
  editor->m_reactivate_request = true;
}

void
EditorSectorsMenu::create_sector()
{
  auto level = Editor::current()->get_level();

  auto new_sector = SectorParser::from_nothing(*level);

  if (!new_sector) {
    log_warning << "Failed to create a new sector." << std::endl;
    return;
  }

  // Find an unique name.
  std::string sector_name;
  int num = 2;
  do {
    sector_name = "sector" + std::to_string(num);
    num++;
  } while ( level->get_sector(sector_name) );
  new_sector->set_name(sector_name);

  level->add_sector(std::move(new_sector));
  Editor::current()->load_sector(sector_name);
  MenuManager::instance().clear_menu_stack();
  Editor::current()->m_reactivate_request = true;
}

void
EditorSectorsMenu::delete_sector()
{
  Level* level = Editor::current()->get_level();
  auto dialog = std::make_unique<Dialog>();

  // Do not delete sector when there would be no left.
  if (level->get_sector_count() < 2) {
    dialog->set_text(_("Each level must have at least one sector."));
    dialog->clear_buttons();
    dialog->add_cancel_button(_("Cancel"));
  } else {
    // Confirmation dialog.
    dialog->set_text(_("Do you really want to delete this sector?"));
    dialog->clear_buttons();
    dialog->add_cancel_button(_("Cancel"));
    dialog->add_button(_("Delete sector"), [] {
        MenuManager::instance().clear_menu_stack();
        Editor::current()->delete_current_sector();
      });
  }
  MenuManager::instance().set_dialog(std::move(dialog));
}

void
EditorSectorsMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0)
  {
    Level* level = Editor::current()->get_level();
    Sector* sector = level->get_sector(item.get_id());
    Editor::current()->load_sector(sector->get_name());
    MenuManager::instance().clear_menu_stack();
  }
  else
  {
    switch (item.get_id())
    {
      case -1:
        break;

      case -2:
        create_sector();
        break;

      case -3:
        delete_sector();
        break;

      case -4:
        MenuManager::instance().clear_menu_stack();
        break;
    }
  }
}

/* EOF */
