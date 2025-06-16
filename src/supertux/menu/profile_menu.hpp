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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_PROFILE_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_PROFILE_MENU_HPP

#include "gui/menu.hpp"

class Profile;

class ProfileMenu final : public Menu
{
public:
  ProfileMenu();

  void refresh() override;
  void menu_action(MenuItem& item) override;

private:
  void rebuild_menu();

  void on_profile_change();

private:
  std::vector<Profile*> m_profiles;
  Profile* m_current_profile;

private:
  ProfileMenu(const ProfileMenu&) = delete;
  ProfileMenu& operator=(const ProfileMenu&) = delete;
};

#endif

/* EOF */
