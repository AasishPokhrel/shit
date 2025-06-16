//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/menu/addon_menu.hpp"

#include <fmt/format.h>

#include "addon/addon.hpp"
#include "addon/addon_manager.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/addon_browse_menu.hpp"
#include "supertux/menu/addon_file_install_menu.hpp"
#include "supertux/menu/addon_preview_menu.hpp"
#include "supertux/menu/download_dialog.hpp"
#include "util/log.hpp"

#define IS_UPDATE_MENU_ID(idx) (((idx) - MNID_ADDON_LIST_START) % 2 == 0)
#define IS_INSTALLED_MENU_ID(idx) (((idx) - MNID_ADDON_LIST_START) % 2 == 1)

#define MAKE_UPDATE_MENU_ID(idx) (MNID_ADDON_LIST_START + 2 * (idx) + 0)
#define MAKE_INSTALLED_MENU_ID(idx) (MNID_ADDON_LIST_START + 2 * (idx) + 1)

#define UNPACK_UPDATE_MENU_ID(idx) ((((idx) - MNID_ADDON_LIST_START) - 0) / 2)
#define UNPACK_INSTALLED_MENU_ID(idx) ((((idx) - MNID_ADDON_LIST_START) - 1) / 2)

AddonMenu::AddonMenu(bool language_packs_only) :
  m_addon_manager(*AddonManager::current()),
  m_installed_addons(),
  m_addons_enabled(),
  m_langpacks_only(language_packs_only)
{
  refresh();
}

AddonMenu::~AddonMenu()
{
}

void
AddonMenu::refresh()
{
  m_installed_addons = m_addon_manager.get_installed_addons();

  m_addons_enabled.reset(new bool[m_installed_addons.size()]);

  rebuild_menu();
}

void
AddonMenu::rebuild_menu()
{
  clear();
  add_label(m_langpacks_only ? _("Installed Language Packs") : _("Installed Add-ons"));
  add_hl();

  std::vector<int> addon_updates_to_list;
  std::vector<int> addons_to_list;
  bool langpacks_installed = false;
  if (m_installed_addons.empty())
  {
    add_inactive(m_langpacks_only ? _("No language packs installed") : _("No Add-ons installed"));
  }
  else
  {
    int idx = 0;
    for (const auto& addon_id : m_installed_addons)
    {
      const Addon& addon = m_addon_manager.get_installed_addon(addon_id);
      m_addons_enabled[idx] = addon.is_enabled();
      if (addon.is_visible() && ((m_langpacks_only && addon.get_type() == Addon::LANGUAGEPACK) || !m_langpacks_only))
      {
        bool addon_update = false;
        try
        {
          // Detect if the add-on has an update.
          const Addon& repository_addon = m_addon_manager.get_repository_addon(addon_id);
          if (addon.get_md5() != repository_addon.get_md5() &&
            addon.get_version() < repository_addon.get_version())
          {
            log_debug << addon.get_id() << " is installed, but updated: '"
                      << addon.get_md5() << "' vs '" << repository_addon.get_md5() << "'  '"
                      << addon.get_version() << "' vs '" << repository_addon.get_version() << "'"
                      << std::endl;
            addon_update = true;
            if (addon.get_type() == Addon::LANGUAGEPACK) langpacks_installed = true;
            addon_updates_to_list.push_back(idx);
          }
        }
        catch (std::exception& e)
        {
          log_warning << "Installed addon not available in repository: " << e.what() << std::endl;
        }
        if (!addon_update)
        {
          // Save the current installed addon for printing
          if (addon.get_type() == Addon::LANGUAGEPACK) langpacks_installed = true;
          addons_to_list.push_back(idx);
        }
      }
      idx += 1;
    }
    if (!langpacks_installed && m_langpacks_only) add_inactive(_("No language packs installed"));
  }

  for (const auto& index : addon_updates_to_list)
  {
    const Addon& addon = m_addon_manager.get_installed_addon(m_installed_addons[index]);
    const std::string text = addon_string_util::generate_menu_item_text(addon);
    if(addon.is_enabled())
      add_entry(MAKE_UPDATE_MENU_ID(index), fmt::format(_("{} *UPDATE*"), text));
    else
      add_entry(MAKE_UPDATE_MENU_ID(index), fmt::format(_("{} [DISABLED] *UPDATE*"), text));
  }
  for (const auto& index : addons_to_list)
  {
    const Addon& addon = m_addon_manager.get_installed_addon(m_installed_addons[index]);
    const std::string text = addon_string_util::generate_menu_item_text(addon);
    if(addon.is_enabled())
      add_entry(MAKE_INSTALLED_MENU_ID(index), fmt::format("{}", text));
    else
      add_entry(MAKE_INSTALLED_MENU_ID(index), fmt::format(_("{} [DISABLED]"), text));
  }

  add_hl();

  if ((m_langpacks_only && langpacks_installed) || (!m_langpacks_only && m_installed_addons.size() > 0))
  {
    const auto addon_updates_count = addon_updates_to_list.size();
    if (addon_updates_count == 0)
    {
      add_inactive(_("No updates available."));
    }
    else
    {
      add_inactive(fmt::format(fmt::runtime(_("{} {} available")), addon_updates_count, addon_updates_count == 1 ? _("update") : _("updates")));
    }
    add_entry(MNID_CHECK_ONLINE, _("Check for updates"));
    add_hl();
  }

  add_entry(MNID_BROWSE, m_langpacks_only ? _("Browse language packs") : _("Browse Add-ons"));
  add_entry(MNID_INSTALL_FROM_FILE, _("Install from file"));
  add_hl();
  add_back(_("Back"));
}

void
AddonMenu::menu_action(MenuItem& item)
{
  int index = item.get_id();
  if (index == MNID_CHECK_ONLINE)
  {
    check_for_updates();
  }
  else if (index == MNID_BROWSE)
  {
    if (g_config->disable_network)
      Dialog::show_message(_("To browse through the add-on catalog, you must enable networking."));
    else
      MenuManager::instance().push_menu(std::make_unique<AddonBrowseMenu>(m_langpacks_only, false));
  }
  else if (index == MNID_INSTALL_FROM_FILE)
  { 
    MenuManager::instance().push_menu(std::make_unique<AddonFileInstallMenu>(this));
  }
  else if (IS_UPDATE_MENU_ID(index))
  {
    int idx = UNPACK_UPDATE_MENU_ID(index);
    if (0 <= idx && idx < static_cast<int>(m_installed_addons.size()))
    {
      const Addon& addon = m_addon_manager.get_installed_addon(m_installed_addons[idx]);
      MenuManager::instance().push_menu(std::make_unique<AddonPreviewMenu>(addon, false, true));
    }
  }
  else if (IS_INSTALLED_MENU_ID(index))
  {
    int idx = UNPACK_INSTALLED_MENU_ID(index);
    if (0 <= idx && idx < static_cast<int>(m_installed_addons.size()))
    {
      const Addon& addon = m_addon_manager.get_installed_addon(m_installed_addons[idx]);
      MenuManager::instance().push_menu(std::make_unique<AddonPreviewMenu>(addon));
    }
  }
  else
  {
    log_warning << "Unknown menu item clicked: " << index << std::endl;
  }
}

void
AddonMenu::check_for_updates()
{
  if (g_config->disable_network)
  {
    Dialog::show_message(_("To check for add-on updates, you must enable networking."));
    return;
  }

  try
  {
    TransferStatusPtr status = m_addon_manager.request_check_online();
    auto dialog = std::make_unique<DownloadDialog>(status, false);
    dialog->set_title(_("Checking for updates..."));
    MenuManager::instance().set_dialog(std::move(dialog));
    status->then([this](bool success)
    {
      if (success) refresh();
      set_active_item(MNID_CHECK_ONLINE);
    });
  }
  catch (std::exception& e)
  {
    log_warning << "Check for available Add-ons failed: " << e.what() << std::endl;
  }
}

/* EOF */
