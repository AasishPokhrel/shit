//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#ifndef HEADER_SUPERTUX_SUPERTUX_PROFILE_MANAGER_HPP
#define HEADER_SUPERTUX_SUPERTUX_PROFILE_MANAGER_HPP

#include "util/currenton.hpp"

#include <map>
#include <memory>
#include <vector>

#include "supertux/profile.hpp"

class ProfileManager final : public Currenton<ProfileManager>
{
public:
  ProfileManager();

  Profile& get_current_profile();
  Profile& get_profile(int id);
  std::vector<Profile*> get_profiles();

  void reset_profile(int id);
  void delete_profile(int id);

private:
  std::map<int, std::unique_ptr<Profile>> m_profiles;

private:
  ProfileManager(const ProfileManager&) = delete;
  ProfileManager& operator=(const ProfileManager&) = delete;
};

#endif

/* EOF */
