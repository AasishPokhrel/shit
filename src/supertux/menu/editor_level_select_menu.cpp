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

#include "supertux/menu/editor_level_select_menu.hpp"

#include <physfs.h>

#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/level.hpp"
#include "supertux/level_parser.hpp"
#include "supertux/levelset.hpp"
#include "supertux/menu/editor_levelset_menu.hpp"
#include "supertux/menu/editor_delete_level_menu.hpp"
#include "supertux/menu/editor_levelset_select_menu.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"

EditorLevelSelectMenu::EditorLevelSelectMenu() :
  m_world(),
  m_levelset(),
  m_levelset_select_menu()
{
  initialize();
}

EditorLevelSelectMenu::EditorLevelSelectMenu(std::unique_ptr<World> world, EditorLevelsetSelectMenu* levelset_select_menu) :
  m_world(std::move(world)),
  m_levelset(),
  m_levelset_select_menu(levelset_select_menu)
{
  initialize();
}

void
EditorLevelSelectMenu::reload_menu()
{
  clear();
  initialize();
}

void
EditorLevelSelectMenu::initialize()
{
  Editor::current()->m_deactivate_request = true;

  World* world = get_world();
  auto basedir = world->get_basedir();
  m_levelset = std::unique_ptr<Levelset>(new Levelset(basedir, /* recursively = */ true));
  auto num_levels = m_levelset->get_num_levels();

  add_label(world->get_title());
  add_hl();

  if (num_levels == 0)
  {
    add_inactive(_("Empty World"));
  }
  else
  {
    for (int i = 0; i < num_levels; ++i)
    {
      std::string filename = m_levelset->get_level_filename(i);
      std::string full_filename = FileSystem::join(basedir, filename);
      std::string title = LevelParser::get_level_name(full_filename);
      add_entry(i, title);
    }
  }

  add_hl();
  
  add_entry(-1, _("Create Level"));

  std::string worldmap_file = FileSystem::join(basedir, "worldmap.stwm");
  if (PHYSFS_exists(worldmap_file.c_str())) {
    add_entry(-4, _("Edit Worldmap"));
  } else {
    add_entry(-6, _("Create Worldmap"));
  }
  add_entry(-5,_("Delete level"));
  add_hl();
  add_entry(-3, _("World Settings"));
  add_back(_("Back"),-2);
}

EditorLevelSelectMenu::~EditorLevelSelectMenu()
{
  auto editor = Editor::current();
  if (editor == nullptr) {
    return;
  }
  editor->m_reactivate_request = true;
}

World*
EditorLevelSelectMenu::get_world() const
{
  return m_world ? m_world.get() : Editor::current()->get_world();
}

void
EditorLevelSelectMenu::create_level()
{
  create_item(false);
}

void
EditorLevelSelectMenu::create_worldmap()
{
  create_item(true);
}

void
EditorLevelSelectMenu::create_item(bool worldmap)
{
  World* world = get_world();
  auto basedir = world->get_basedir();
  auto new_item = worldmap ?
      LevelParser::from_nothing_worldmap(basedir, world->get_title()) :
      LevelParser::from_nothing(basedir);
  new_item->save(FileSystem::join(basedir, new_item->m_filename));
  open_level(new_item->m_filename);

  if (worldmap)
  {
    Dialog::show_message(_("Share this worldmap under license CC-BY-SA 4.0 International (advised).\n"
                           "It allows modifications and redistribution by third-parties.\nIf you don't "
                           "agree with this license, change it in worldmap properties.\nDISCLAIMER: The "
                           "SuperTux authors take no responsibility for your choice of license."));
  }
  else
  {
    Dialog::show_message(_("Share this level under license CC-BY-SA 4.0 International (advised).\n"
                           "It allows modifications and redistribution by third-parties.\nIf you don't "
                           "agree with this license, change it in level properties.\nDISCLAIMER: The "
                           "SuperTux authors take no responsibility for your choice of license."));
  }
}

void
EditorLevelSelectMenu::open_level(const std::string& filename)
{
  auto editor = Editor::current();

  // If a world has been provided (not associated with the editor), set it as the editor world.
  if (m_world)
    editor->set_world(std::move(m_world));

  editor->set_level(filename);
  MenuManager::instance().clear_menu_stack();
}

void
EditorLevelSelectMenu::menu_action(MenuItem& item)
{
  auto editor = Editor::current();
  if (item.get_id() >= 0)
  {
    std::string file_name = m_levelset->get_level_filename(item.get_id());
    std::string file_name_full = FileSystem::join(editor->get_level_directory(), file_name);

    if (PHYSFS_exists((file_name_full + "~").c_str())) {
      auto dialog = std::make_unique<Dialog>(/* passive = */ false, /* auto_clear_dialogs = */ false);
      dialog->set_text(_("An auto-save recovery file was found. Would you like to restore the recovery\nfile and resume where you were before the editor crashed?"));
      dialog->clear_buttons();
      dialog->add_default_button(_("Yes"), [this, file_name] {
        open_level(file_name + "~");
        MenuManager::instance().set_dialog({});
      });
      dialog->add_button(_("No"), [this, file_name] {
        Dialog::show_confirmation(_("This will delete the auto-save file. Are you sure?"), [this, file_name] {
          open_level(file_name);
        });
      });
      dialog->add_cancel_button(_("Cancel"));
      MenuManager::instance().set_dialog(std::move(dialog));
    } else {
      open_level(file_name);
    }

  } else {
    switch (item.get_id()) {
      case -1:
        create_level();
        break;
      case -2:
        MenuManager::instance().pop_menu();
        break;
      case -3: {
        auto menu = std::unique_ptr<Menu>(new EditorLevelsetMenu(get_world()));
        MenuManager::instance().push_menu(std::move(menu));
      } break;
      case -4:
        open_level("worldmap.stwm");
        break;
      case -5: {
        if (m_levelset->get_num_levels() > 0)
        {
          auto delete_menu = std::unique_ptr<Menu>(new EditorDeleteLevelMenu(get_world(), m_levelset.get(), this, m_levelset_select_menu));
          MenuManager::instance().push_menu(std::move(delete_menu));
        }
        break;
      }
      case -6:
        create_worldmap();
        break;
      default:
        break;
    }
  }
}

/* EOF */
