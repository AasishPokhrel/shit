//  SuperTux
//  Copyright (C) 2022-2023 Vankata453
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

#include "supertux/menu/addon_preview_menu.hpp"

#include <fmt/format.h>

#include "addon/addon.hpp"
#include "addon/addon_manager.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/download_dialog.hpp"
#include "supertux/resources.hpp"
#include "util/log.hpp"

AddonPreviewMenu::AddonPreviewMenu(const Addon& addon, bool auto_install, bool update) :
  m_addon_manager(*AddonManager::current()),
  m_addon(addon),
  m_auto_install(auto_install),
  m_update(update),
  m_addon_enabled(addon.is_enabled()),
  m_show_screenshots(false),
  m_screenshot_download_status(),
  m_screenshot_download_success(false)
{
  if (m_auto_install)
  {
    install_addon();
    return;
  }
  rebuild_menu();
}

AddonPreviewMenu::~AddonPreviewMenu()
{
}

void
AddonPreviewMenu::rebuild_menu()
{
  std::string author;
  std::string type;
  std::string desc;
  std::string license;
  std::vector<std::string> dependencies;
  bool overrides_data = false;
  bool screenshots_available = false;
  bool info_unavailable = false;
  try
  {
    const Addon& repository_addon = m_addon_manager.get_repository_addon(m_addon.get_id());
    author = repository_addon.get_author();
    type = addon_string_util::addon_type_to_translated_string(repository_addon.get_type());
    desc = repository_addon.get_description();
    license = repository_addon.get_license();
    dependencies = repository_addon.get_dependencies();
    overrides_data = repository_addon.overrides_data();
    screenshots_available = repository_addon.get_screenshots().size() > 0;
  }
  catch (std::exception& err)
  {
    log_warning << "Installed addon not available in repository: " << err.what() << std::endl;
    author = m_addon.get_author();
    type = addon_string_util::addon_type_to_translated_string(m_addon.get_type());
    license = m_addon.get_license();
    overrides_data = m_addon.overrides_data();
    info_unavailable = true;
  }

  clear();

  add_label(fmt::format(fmt::runtime(_("{} \"{}\"")), type, m_addon.get_title()));
  add_hl();

  if (info_unavailable)
  {
    add_inactive(_("Some information about this add-on is not available."));
    add_inactive(_("Perform a \"Check Online\" to try retrieving it."));
    add_hl();
  }

  add_inactive(author.empty() ? _("No author specified.") : fmt::format(fmt::runtime(_("Author: {}")), author), !author.empty());
  add_inactive(fmt::format(fmt::runtime(_("Type: {}")), type), true);
  add_inactive(license.empty() ? _("No license specified.") : fmt::format(fmt::runtime(_("License: {}")), license), !license.empty());
  add_inactive("");

  if (!dependencies.empty())
  {
    add_inactive(_("Dependencies:"), true);
    for (const std::string& id : dependencies)
    {
      std::string text;
      try
      {
        const Addon& dependency = m_addon_manager.get_repository_addon(id);
        text = fmt::format(fmt::runtime("\"{}\" ({}): {}"), dependency.get_title(),
                            addon_string_util::addon_type_to_translated_string(dependency.get_type()),
                            m_addon_manager.is_addon_installed(id) ? _("Installed") : _("Not installed"));
      }
      catch (std::exception& err)
      {
        log_warning << "Dependency not available in repository: " << err.what() << std::endl;
        text = fmt::format(fmt::runtime("\"{}\": {}"), id, _("Not available!"));
      }
      add_inactive(text, true);
    }
    add_inactive("");
  }

  add_inactive(_("Description:"), true);
  if (desc.empty())
  {
    add_inactive(_("No description available."));
  }
  else
  {
    std::string overflow;
    add_inactive(Resources::normal_font->wrap_to_width(desc, 600.f, &overflow), true);
    while (!overflow.empty())
    {
      add_inactive(Resources::normal_font->wrap_to_width(overflow, 600.f, &overflow), true);
    }
  }
  add_inactive("");

  add_inactive(_("Overrides Data: ") + (overrides_data ? _("Yes") : _("No")), true);
  add_inactive("");

  if (screenshots_available && !m_auto_install)
  {
    if (m_show_screenshots)
    {
      if (m_screenshot_download_success)
      {
        add_images(m_addon_manager.get_local_addon_screenshots(m_addon.get_id()),
                   426, 240, MNID_SCREENSHOTS);
      }
      else
      {
        add_inactive(_("Failed to load all available screenshot previews."));
      }
    }
    else if (g_config->disable_network)
    {
      add_inactive(_("To fetch add-on screenshots, you must enable networking."));
    }
    else
    {
      const std::string show_screenshots_text = _("Show screenshots");
      if (m_auto_install)
      {
        add_inactive(show_screenshots_text);
      }
      else
      {
        add_entry(MNID_SHOW_SCREENSHOTS, show_screenshots_text);
      }
    }
  }
  else if (m_auto_install)
  {
    add_inactive(_("Screenshot previews are disabled for automatic installs."));
  }
  else
  {
    add_inactive(_("No screenshot previews available."));
  }

  add_hl();

  bool addon_installed = m_addon.is_installed();
  if ((!addon_installed || m_update) && !info_unavailable)
  {
    const std::string action = m_update ? _("Update") : _("Install");
    if (m_auto_install)
    {
      add_inactive(action);
    }
    else
    {
      add_entry(MNID_INSTALL, action);
    }
  }
  if (addon_installed)
  {
    add_toggle(MNID_TOGGLE, _("Enabled"), &m_addon_enabled, true);
    add_entry(MNID_UNINSTALL, _("Uninstall"));
  }

  add_back(_("Back"));
}

void
AddonPreviewMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_SCREENSHOTS: // Non-interactive item.
      return;

    case MNID_SHOW_SCREENSHOTS:
      show_screenshots();
      break;

    case MNID_INSTALL:
      install_addon();
      break;

    case MNID_UNINSTALL:
    {
      std::string confirmation_message = fmt::format(fmt::runtime(_("Are you sure you want to uninstall \"{}\"?")), m_addon.get_title());
      if (m_addon.is_levelset()) confirmation_message += _("\nYour progress won't be lost.");

      Dialog::show_confirmation(confirmation_message, [this]()
      {
        const std::vector<std::string> depending_addons = m_addon_manager.get_depending_addons(m_addon.get_id());
        if (depending_addons.empty())
        {
          uninstall_addon();
        }
        else
        {
          // Other add-ons depend on the add-on that's being uninstalled.
          const std::string dependency_message = fmt::format(fmt::runtime(_("NOTE: The add-on \"{}\" is a dependency of {} other installed {}.\nAre you sure you wish to uninstall?")),
              m_addon.get_title(), depending_addons.size(), addon_string_util::get_addon_plural_form(depending_addons.size()));
          Dialog::show_confirmation(dependency_message, [this]()
          {
            uninstall_addon();
          }, true);
        }
      }, true);

      break;
    }

    case MNID_TOGGLE:
      try
      {
        toggle_addon();
      }
      catch (std::exception& err)
      {
        Dialog::show_message(fmt::format(fmt::runtime(_("Cannot toggle add-on \"{}\":\n{}")), m_addon.get_id(), err.what()));
        m_addon_enabled = !m_addon_enabled;
      }
      break;

    default:
      log_warning << "Unknown menu item clicked: " << item.get_id() << std::endl;
      break;
  }
}

void
AddonPreviewMenu::show_screenshots()
{
  if (m_screenshot_download_status &&
      m_screenshot_download_status->is_active()) // If a download is currently running.
  {
    log_warning << "A download is currently active. Ignoring screenshot download request." << std::endl;
    return;
  }

  m_screenshot_download_status = m_addon_manager.request_download_addon_screenshots(m_addon.get_id());

  if (!m_screenshot_download_status->is_active()) // If no screenshots have been scheduled for download (all are downloaded).
  {
    // Directly reload the menu with screenshots.
    m_screenshot_download_success = true;
    m_show_screenshots = true;
    rebuild_menu();
    return;
  }

  auto dialog = std::make_unique<DownloadDialog>(m_screenshot_download_status, true, true);
  dialog->set_title(_("Fetching screenshot previews..."));
  m_screenshot_download_status->then([this](bool success)
  {
    m_screenshot_download_status = nullptr;
    log_info << "Fetching screenshots for add-on \"" << m_addon.get_id() << "\" "
             << (success ? "finished successfully" : "failed") << "." << std::endl;

    // Reload the menu with screenshots.
    m_screenshot_download_success = success;
    m_show_screenshots = true;
    rebuild_menu();
  });
  MenuManager::instance().set_dialog(std::move(dialog));
}

void
AddonPreviewMenu::install_addon()
{
  auto addon_id = m_addon.get_id();
  TransferStatusListPtr status = m_addon_manager.request_install_addon(addon_id);
  auto dialog = std::make_unique<DownloadDialog>(status, false);
  const std::string action = m_update ? _("Updating") : _("Downloading");
  const Addon& repository_addon = m_addon_manager.get_repository_addon(addon_id);
  dialog->set_title(fmt::format(fmt::runtime("{} {}"), action, addon_string_util::generate_menu_item_text(repository_addon)));
  status->then([this, addon_id](bool success)
  {
    if (m_auto_install)
    {
      MenuManager::instance().set_dialog({});
      MenuManager::instance().clear_menu_stack();
      return;
    }
    if (success)
    {
      MenuManager::instance().pop_menu(true);
      if (!m_update) MenuManager::instance().pop_menu(true);
      MenuManager::instance().current_menu()->refresh();
    }
  });
  MenuManager::instance().set_dialog(std::move(dialog));
}

void
AddonPreviewMenu::uninstall_addon()
{
  const AddonId& addon_id = m_addon.get_id();
  try
  {
    m_addon_manager.uninstall_addon(addon_id);
    Dialog::show_message(_("Add-on uninstalled successfully."));
  }
  catch (std::exception& err)
  {
    log_warning << "Error uninstalling add-on: " << err.what() << std::endl;
    Dialog::show_message(fmt::format(fmt::runtime(_("Error uninstalling add-on:\n{}")), err.what()));
  }
  MenuManager::instance().pop_menu(true);
  MenuManager::instance().current_menu()->refresh();
}

void
AddonPreviewMenu::toggle_addon()
{
  const AddonId& addon_id = m_addon.get_id();
  try
  {
    if (m_addon.is_enabled())
    {
      m_addon_manager.disable_addon(addon_id);
    }
    else
    {
      m_addon_manager.enable_addon(addon_id);
    }
  }
  catch (std::exception& err)
  {
    throw std::runtime_error(err.what());
  }

  if (m_addon.requires_restart())
  {
    Dialog::show_message(_("Please restart SuperTux\nfor these changes to take effect."));
  }
  MenuManager::instance().previous_menu()->refresh();
}

/* EOF */
