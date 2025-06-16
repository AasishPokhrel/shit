//  SuperTux
//  Copyright (C) 2021 mrkubax10 <mrkubax10@onet.pl>
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

#include "supertux/menu/editor_delete_level_menu.hpp"

#include <physfs.h>
#include <fmt/format.h>

#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "supertux/level.hpp"
#include "supertux/level_parser.hpp"
#include "supertux/levelset.hpp"
#include "supertux/menu/editor_level_select_menu.hpp"
#include "supertux/menu/editor_levelset_select_menu.hpp"
#include "util/file_system.hpp"

EditorDeleteLevelMenu::EditorDeleteLevelMenu(World* world, Levelset* levelset,
                                             EditorLevelSelectMenu* level_select_menu, EditorLevelsetSelectMenu* levelset_select_menu) :
  m_levelset(levelset),
  m_level_full_paths(),
  m_level_names(),
  m_level_select_menu(level_select_menu),
  m_levelset_select_menu(levelset_select_menu)
{
  for (int i = 0; i < levelset->get_num_levels(); i++)
  {
    std::string filename = levelset->get_level_filename(i);
    std::string fullpath = FileSystem::join(world->get_basedir(), filename);
    m_level_full_paths.push_back(fullpath);
    const std::string& level_name = LevelParser::get_level_name(fullpath);
    m_level_names.push_back(level_name);
  }
  refresh();
}

void
EditorDeleteLevelMenu::refresh()
{
  clear();
  add_label(_("Delete level"));
  add_hl();
  if (m_levelset->get_num_levels() == 0)
  {
    add_inactive(_("No levels available"));
  }
  for (size_t i = 0; i < m_level_names.size(); i++)
  {
    const std::string& level_name = m_level_names[i];
    if (!level_name.empty())
      add_entry(static_cast<int>(i), level_name);
  }
  add_hl();
  add_back(_("Back"));
}

void
EditorDeleteLevelMenu::menu_action(MenuItem& item)
{
  int id = item.get_id();
  // Cast to avoid compilation warning
  if (id >= 0 && id < static_cast<int>(m_level_full_paths.size()))
  {
    if (Editor::current()->is_level_loaded() && m_level_full_paths[id] == Editor::current()->get_level()->m_filename)
      Dialog::show_message(_("You cannot delete the level that you are editing!"));
    else
    {
      Dialog::show_confirmation(fmt::format(_("You are about to delete level \"{}\". Are you sure?"), m_level_names[id]), [this, id]()
      {
        PHYSFS_delete(m_level_full_paths[id].c_str());
        delete_item(id + 2);
        m_level_full_paths[id].clear();
        m_level_names[id].clear();
        refresh();
        m_level_select_menu->reload_menu();
        if (!Editor::current()->is_level_loaded())
          m_levelset_select_menu->reload_menu();
      });
    }
  }
}
/* EOF */
