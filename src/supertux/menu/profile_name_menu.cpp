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

#include "supertux/menu/profile_name_menu.hpp"

#include <fmt/format.h>

#include "gui/dialog.hpp"
#include "gui/item_textfield.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/profile.hpp"
#include "supertux/profile_manager.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

ProfileNameMenu::ProfileNameMenu(Profile* profile) :
  m_profile(profile),
  m_profile_name()
{
  add_label(m_profile ? fmt::format(fmt::runtime(_("Rename \"{}\"")), m_profile->get_name()) : _("Add profile"));
  add_hl();

  add_textfield(_("Name"), &m_profile_name)
    .set_help(_("Profile names must have a maximum of 20 characters."));

  add_entry(1, m_profile ? _("Rename") : _("Create"));

  add_hl();
  add_back(_("Back"));
}

void
ProfileNameMenu::menu_action(MenuItem& item)
{
  if (item.get_id() <= 0)
    return;

  if (m_profile_name.size() > 20)
  {
    Dialog::show_message(_("Profile names must have a maximum of 20 characters.\nPlease choose a different name."));
    return;
  }

  if (!m_profile) /** Add profile */
  {
    //Find the smallest available profile ID.
    const auto profiles = ProfileManager::current()->get_profiles();
    int id = 1;
    if (!profiles.empty())
      id = profiles.back()->get_id() + 1;

    try
    {
      Profile& profile = ProfileManager::current()->get_profile(id);
      profile.set_name(m_profile_name);
      profile.save();
    }
    catch (const std::exception& err)
    {
      log_warning << "Error creating profile " << id << ": " << err.what() << std::endl;
      Dialog::show_message(_("An error occurred while creating the profile."));
      return;
    }

    g_config->profile = id;
  }
  else /** Rename profile */
  {
    m_profile->set_name(m_profile_name);
    m_profile->save();
  }

  MenuManager::instance().pop_menu();
  MenuManager::instance().current_menu()->refresh();
}

/* EOF */
