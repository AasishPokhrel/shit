//  SuperTux
//  Copyright (C) 2023 mrkubax10 <mrkubax10@onet.pl>
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

#include "supertux/menu/addon_file_install_menu.hpp"

#include "addon/addon_manager.hpp"
#include "supertux/menu/addon_menu.hpp"
#include "util/gettext.hpp"

AddonFileInstallMenu::AddonFileInstallMenu(AddonMenu* addon_menu) :
  m_addon_menu(addon_menu)
{
  refresh();
}

void
AddonFileInstallMenu::refresh()
{
  add_label(_("Install Add-on from file"));
  add_hl();
  add_inactive(_("Drag and drop add-on ZIP archive"));
  add_hl();
  add_back(_("Back"));
}

void
AddonFileInstallMenu::event(const SDL_Event& event)
{
  if (event.type == SDL_DROPFILE)
  {
    char* filename = event.drop.file;
    AddonManager::current()->install_addon_from_local_file(std::string(filename));
    SDL_free(filename);
    m_addon_menu->refresh();
  }
  Menu::event(event);
}

/* EOF */
