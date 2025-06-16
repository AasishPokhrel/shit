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
#include <sstream>
#include <fmt/format.h>

#include "editor/editor.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "physfs/util.hpp"
#include "supertux/levelset.hpp"
#include "supertux/menu/editor_levelset_select_menu.hpp"
#include "supertux/menu/editor_delete_levelset_menu.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

EditorLevelsetSelectMenu::EditorLevelsetSelectMenu() :
  m_contrib_worlds()
{
  initialize();
}

EditorLevelsetSelectMenu::~EditorLevelsetSelectMenu()
{
  auto editor = Editor::current();
  if (editor == nullptr) {
    return;
  }
  if (!editor->is_level_loaded() && !editor->m_reload_request) {
    editor->m_quit_request = true;
  } else {
    editor->m_reactivate_request = true;
  }
}
void
EditorLevelsetSelectMenu::initialize()
{
  Editor::current()->m_deactivate_request = true;
  m_contrib_worlds.clear();

  // Generating contrib levels list by making use of Level Subset
  std::vector<std::string> level_worlds;
  
  physfsutil::enumerate_files_alphabetical("levels", [&level_worlds](const auto& filename) {
    std::string filepath = FileSystem::join("levels", filename);
    if (physfsutil::is_directory(filepath))
    {
      level_worlds.push_back(filepath);
    }
    return false;
  });

  add_label(_("Choose World"));
  add_hl();

  int i = 0;
  for (const auto& level_world : level_worlds)
  {
    try
    {
      std::unique_ptr<World> world = World::from_directory(level_world);
      if (world->hide_from_contribs())
      {
        continue;
      }
      if (!world->is_levelset() && !world->is_worldmap())
      {
        log_warning << level_world << ": Unknown World type." << std::endl;
        continue;
      }
      auto title = world->get_title();
      if (title.empty())
      {
        continue;
      }
      auto levelset = std::unique_ptr<Levelset>(
                          new Levelset(level_world, /* recursively = */ true));
      int level_count = levelset->get_num_levels();
      std::ostringstream level_title;
      level_title << title << " ("
                  << fmt::format(fmt::runtime(__("{} level", "{} levels", level_count)), level_count)
                  << ")";
      add_entry(i++, level_title.str());
      m_contrib_worlds.push_back(level_world);
    }
    catch(std::exception& e)
    {
      log_info << "Couldn't parse levelset info for '" << level_world << "': "
               << e.what() << std::endl;
    }
  }

  add_hl();
  add_submenu(_("Create World"), MenuStorage::EDITOR_NEW_LEVELSET_MENU);
  add_entry(-3, _("Delete World"));
  add_back(_("Back"),-2);
}
void EditorLevelsetSelectMenu::reload_menu()
{
  clear();
  initialize();
}

void
EditorLevelsetSelectMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0)
  {
    std::unique_ptr<Menu> menu = std::unique_ptr<Menu>(new EditorLevelSelectMenu(
                                 World::from_directory(m_contrib_worlds[item.get_id()]), this));
    MenuManager::instance().push_menu(std::move(menu));
  }
  else if (item.get_id() == -3)
  {
    std::unique_ptr<EditorDeleteLevelsetMenu> menu = std::make_unique<EditorDeleteLevelsetMenu>(this);
    MenuManager::instance().push_menu(std::move(menu));
  }
}

/* EOF */
