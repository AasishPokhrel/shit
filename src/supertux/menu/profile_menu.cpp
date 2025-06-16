//  SuperTux
//  Copyright (C) 2008 Ingo Ruhnke <grumbel@gmail.com>
//                2022 Vankata453
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

#include "supertux/menu/profile_menu.hpp"

#include <fmt/format.h>
#include <sstream>

#include "gui/dialog.hpp"
#include "gui/menu_manager.hpp"
#include "gui/menu_item.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/profile_name_menu.hpp"
#include "supertux/profile_manager.hpp"
#include "supertux/title_screen.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

ProfileMenu::ProfileMenu() :
  m_profiles(),
  m_current_profile()
{
  refresh();
}

void
ProfileMenu::refresh()
{
  m_profiles = ProfileManager::current()->get_profiles();
  if (m_current_profile && g_config->profile != m_current_profile->get_id())
    on_profile_change();

  rebuild_menu();
}

void
ProfileMenu::rebuild_menu()
{
  clear();
  add_label(_("Select Profile"));

  add_hl();
  if (m_profiles.empty())
    add_inactive(_("No profiles found."));

  for (auto* profile : m_profiles)
  {
    const int id = profile->get_id();
    const std::string name = profile->get_name();
    const bool current = (id == g_config->profile);

    std::string text = (name.empty() ? fmt::format(fmt::runtime(_("Profile {}")), id) :
                        (g_config->developer_mode ? fmt::format(fmt::runtime(_("{} (Profile {})")), name, id) : name));
    if (current)
      text = "[" + text + "]";

    add_entry(id, text);
    if (current)
    {
      set_active_item(id);
      m_current_profile = profile;
    }
  }

  if (!m_current_profile && !m_profiles.empty())
  {
    add_hl();
    add_inactive(_("No profile selected."));
  }

  add_hl();
  add_entry(-1, _("Add"));

  if (m_current_profile)
    add_entry(-2, _("Rename"));
  else
    add_inactive(_("Rename"));

  add_hl();

  if (m_current_profile)
    add_entry(-3, _("Reset"));
  else
    add_inactive(_("Reset"));

  if (!m_profiles.empty())
    add_entry(-4, _("Reset all"));
  else
    add_inactive(_("Reset all"));

  if (m_current_profile)
    add_entry(-5, _("Delete"));
  else
    add_inactive(_("Delete"));

  if (!m_profiles.empty())
    add_entry(-6, _("Delete all"));
  else
    add_inactive(_("Delete all"));

  add_hl();
  add_back(_("Back"));
}

void
ProfileMenu::menu_action(MenuItem& item)
{
  const auto& id = item.get_id();
  if (id > 0)
  {
    if (g_config->profile == id)
    {
      MenuManager::instance().clear_menu_stack();
      return;
    }
    g_config->profile = id;
    on_profile_change();
    rebuild_menu();
  }
  else if (id == -1)
  {
    MenuManager::instance().push_menu(std::make_unique<ProfileNameMenu>());
  }
  else if (id == -2)
  {
    MenuManager::instance().push_menu(std::make_unique<ProfileNameMenu>(m_current_profile));
  }
  else if (id == -3)
  {
    const std::string name = m_current_profile->get_name();
    const std::string message = fmt::format(
      fmt::runtime(_("This will reset all game progress on the profile \"{}\".\nAre you sure?")),
      name.empty() ? fmt::format(fmt::runtime(_("Profile {}")), m_current_profile->get_id()) : name);

    Dialog::show_confirmation(message, []() {
      ProfileManager::current()->reset_profile(g_config->profile);
    });
  }
  else if (id == -4)
  {
    Dialog::show_confirmation(_("This will reset your game progress on all profiles. Are you sure?"), [this]() {
      auto* manager = ProfileManager::current();
      for (auto* profile : m_profiles)
        manager->reset_profile(profile->get_id());
    });
  }
  else if (id == -5)
  {
    const std::string name = m_current_profile->get_name();
    const std::string message = fmt::format(
      fmt::runtime(_("This will delete the profile \"{}\",\nincluding all game progress on it. Are you sure?")),
      name.empty() ? fmt::format(fmt::runtime(_("Profile {}")), m_current_profile->get_id()) : name);

    Dialog::show_confirmation(message, [this]() {
      ProfileManager::current()->delete_profile(g_config->profile);
      g_config->profile = 1;
      refresh();
    });
  }
  else if (id == -6)
  {
    Dialog::show_confirmation(_("This will delete all profiles, including all game progress on them.\nAre you sure?"), [this]() {
      auto* manager = ProfileManager::current();
      for (auto* profile : m_profiles)
        manager->delete_profile(profile->get_id());

      g_config->profile = 1;
      refresh();
    });
  }
  else
  {
    log_warning << "Unknown menu item with id \"" << id << "\" pressed." << std::endl;
  }
}

void
ProfileMenu::on_profile_change()
{
  /** Perform actions on current profile change */
  TitleScreen::current()->refresh_level();
}

/* EOF */
