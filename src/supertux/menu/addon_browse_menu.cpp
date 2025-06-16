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

#include "supertux/menu/addon_browse_menu.hpp"

#include <fmt/format.h>

#include "addon/addon.hpp"
#include "addon/addon_manager.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/menu/addon_preview_menu.hpp"
#include "supertux/menu/download_dialog.hpp"
#include "util/log.hpp"

#define IS_REPOSITORY_MENU_ID(idx) (((idx) - MNID_ADDON_LIST_START) % 2 == 0)

#define MAKE_REPOSITORY_MENU_ID(idx) (MNID_ADDON_LIST_START + 2 * (idx) + 0)

#define UNPACK_REPOSITORY_MENU_ID(idx) ((((idx) - MNID_ADDON_LIST_START) - 0) / 2)

AddonBrowseMenu::AddonBrowseMenu(bool langpacks_only, bool auto_install_langpack) :
  m_addon_manager(*AddonManager::current()),
  m_repository_addons(),
  m_langpacks_only(langpacks_only),
  m_auto_install_langpack(auto_install_langpack),
  m_browse_page(1),
  m_max_addons_on_page(10)
{
  refresh();
  if (auto_install_langpack)
  {
    const std::string& language = g_dictionary_manager->get_language().get_language();
    if (language == "en")
      return;
    check_online();
  }
}

AddonBrowseMenu::~AddonBrowseMenu()
{
}

void
AddonBrowseMenu::refresh()
{
  m_repository_addons = m_addon_manager.get_repository_addons();

  rebuild_menu();
}

void
AddonBrowseMenu::rebuild_menu()
{
  clear();
  add_label(m_langpacks_only ? _("Browse Language Packs") : _("Browse Add-ons"));
  add_hl();

  int idx = 0;
  int addons_count = 0;
  std::vector<int> addons_to_list;
  for (const auto& addon_id : m_repository_addons)
  {
    const Addon& addon = m_addon_manager.get_repository_addon(addon_id);
    try
    {
      // Add-on is already installed, so check if they are the same.
      Addon& installed_addon = m_addon_manager.get_installed_addon(addon_id);
      if (installed_addon.get_md5() == addon.get_md5() ||
          installed_addon.get_version() > addon.get_version())
      {
        log_debug << "ignoring already installed add-on " << installed_addon.get_id() << std::endl;
      }
    }
    catch(const std::exception&)
    {
      // Add-on is not installed.
      if (addon.is_visible())
      {
        if ((m_langpacks_only && addon.get_type() == Addon::LANGUAGEPACK) || !m_langpacks_only)
        {
          if (addons_count >= m_max_addons_on_page * (m_browse_page - 1) && addons_count < m_max_addons_on_page * m_browse_page) {
            addons_to_list.push_back(idx);
          }
          addons_count++;
        }
      }
    }
    idx += 1;
  }

  int last_browse_page = 0;

  if (addons_count > 0)
  {
    last_browse_page = static_cast<int>(std::ceil(static_cast<double>(addons_count) / static_cast<double>(m_max_addons_on_page)));

    add_inactive(fmt::format(fmt::runtime(_("Page {}/{}")), m_browse_page, last_browse_page));
    add_hl();
  }

  for (const auto& index : addons_to_list)
  {
    const std::string text = addon_string_util::generate_menu_item_text(m_addon_manager.get_repository_addon(m_repository_addons[index]));
    add_entry(MAKE_REPOSITORY_MENU_ID(index), text);
  }

  if (addons_count <= 0 && m_addon_manager.has_been_updated())
  {
    add_inactive(m_langpacks_only ? _("No new language packs available") : _("No new Add-ons available"));
  }
  else if (addons_count <= 0) {
    add_inactive(m_langpacks_only ? _("No language packs available") : _("No Add-ons available"));
  }

  add_hl();
  if (addons_count > 0)
  {
    if (m_browse_page <= 1)
    {
      add_inactive(_("Previous page"));
    }
    else
    {
      add_entry(MNID_PREV_PAGE, _("Previous page"));
    }
    if (m_browse_page >= last_browse_page)
    {
      add_inactive(_("Next page"));
    }
    else
    {
      add_entry(MNID_NEXT_PAGE, _("Next page"));
    }
    add_hl();
  }

  if (!m_addon_manager.has_online_support())
  {
    add_inactive(_("Check Online (disabled)"));
  }
  else
  {
    add_entry(MNID_CHECK_ONLINE, _("Check Online"));
  }

  add_hl();
  add_back(_("Back"));
}

void
AddonBrowseMenu::check_online()
{
  try
  {
    TransferStatusPtr status = m_addon_manager.request_check_online();
    status->then([this](bool success)
    {
      if (success)
      {
        if (m_auto_install_langpack)
        {
          const std::string& langpack_id = "language-pack";
          MenuManager::instance().push_menu(std::make_unique<AddonPreviewMenu>(m_addon_manager.get_repository_addon(langpack_id), true), true);
        }
        else
        {
          refresh();
        }
      }
      else
      {
        if (m_auto_install_langpack)
        {
          MenuManager::instance().set_dialog({});
          MenuManager::instance().clear_menu_stack();
        }
      }
    });
    auto dialog = std::make_unique<DownloadDialog>(status, false);
    dialog->set_title(_("Downloading Add-On Repository Index"));
    MenuManager::instance().set_dialog(std::move(dialog));
  }
  catch (std::exception& e)
  {
    log_warning << "Check for available Add-ons failed: " << e.what() << std::endl;
  }
}

void
AddonBrowseMenu::menu_action(MenuItem& item)
{
  const int index = item.get_id();
  if (index == MNID_CHECK_ONLINE) // Check if "Check Online" was chosen.
  {
    check_online();
  }
  else if (index == MNID_PREV_PAGE || index == MNID_NEXT_PAGE) {
    index == MNID_PREV_PAGE ? m_browse_page-- : m_browse_page++;
    rebuild_menu();
    set_active_item(index);
    if (get_active_item_id() != index) // Check if the item wasn't set as active, because it's disabled.
    {
      set_active_item(index == MNID_PREV_PAGE ? MNID_NEXT_PAGE : MNID_PREV_PAGE);
    }
  }
  else if (IS_REPOSITORY_MENU_ID(index))
  {
    int idx = UNPACK_REPOSITORY_MENU_ID(index);
    if (0 <= idx && idx < static_cast<int>(m_repository_addons.size()))
    {
      const Addon& addon = m_addon_manager.get_repository_addon(m_repository_addons[idx]);
      MenuManager::instance().push_menu(std::make_unique<AddonPreviewMenu>(addon));
    }
  }
  else
  {
    log_warning << "Unknown menu item clicked: " << index << std::endl;
  }
}

/* EOF */
