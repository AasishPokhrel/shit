//  SuperTux
//  Copyright (C) 2022 Vankata453
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_ADDON_BROWSE_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_ADDON_BROWSE_MENU_HPP

#include "gui/menu.hpp"

class Addon;
class AddonManager;

class AddonBrowseMenu final : public Menu
{
private:
  enum {
    MNID_PREV_PAGE = 1,
    MNID_NEXT_PAGE = 2,
    MNID_CHECK_ONLINE = 3,
    MNID_ADDON_LIST_START = 4,
  };

private:
  AddonManager& m_addon_manager;
  std::vector<std::string> m_repository_addons;
  const bool m_langpacks_only;
  const bool m_auto_install_langpack;
  int m_browse_page;
  const int m_max_addons_on_page;

public:
  AddonBrowseMenu(bool langpacks_only, bool auto_install_langpack);
  ~AddonBrowseMenu() override;

  void refresh() override;
  void rebuild_menu();
  void check_online();
  void menu_action(MenuItem& item) override;

private:
  AddonBrowseMenu(const AddonBrowseMenu&) = delete;
  AddonBrowseMenu& operator=(const AddonBrowseMenu&) = delete;
};

#endif

/* EOF */
