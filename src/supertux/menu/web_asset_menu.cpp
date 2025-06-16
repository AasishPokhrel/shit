//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#include "supertux/menu/web_asset_menu.hpp"

#include "gui/dialog.hpp"
#include "gui/menu_filesystem.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "physfs.h"

#include <regex>

WebAssetMenu::WebAssetMenu() :
  m_add_path("")
{
  add_label(_("Manage Assets"));

  add_hl();

  add_textfield(_("New files location"), &m_add_path);
  add_entry(MNID_ADDFILES, _("Add Files"));
  add_entry(MNID_DOWNLOADFILES, _("Download Files"));

  add_hl();

  add_back(_("Back"));
}

void
WebAssetMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_ADDFILES:
    {
      std::regex regex("'");
      m_add_path = std::regex_replace(m_add_path, regex, "\\'");

#ifdef __EMSCRIPTEN__
      emscripten_run_script(("supertux_upload('" + m_add_path + "');").c_str());
#endif
    }
      break;

    case MNID_DOWNLOADFILES:
    {
      std::vector<std::string> empty_vec;
      MenuManager::instance().push_menu(
        std::make_unique<FileSystemMenu>(nullptr, empty_vec, "", false, [](const std::string& file) {
          std::string fullpath(std::string(PHYSFS_getRealDir(file.c_str())) + "/" + file);
          FileSystem::open_path(fullpath);
        })
      );
    }
      break;

    default:
      break;
  }
}

/* EOF */
