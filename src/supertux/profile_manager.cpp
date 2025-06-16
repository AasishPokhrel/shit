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

#include "supertux/profile_manager.hpp"

#include <algorithm>

#include "physfs/util.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"

ProfileManager::ProfileManager() :
  m_profiles()
{
}

Profile&
ProfileManager::get_current_profile()
{
  return get_profile(g_config->profile);
}

Profile&
ProfileManager::get_profile(int id)
{
  auto it = m_profiles.find(id);
  if (it != m_profiles.end())
  {
    return *it->second;
  }
  else
  {
    m_profiles[id] = std::make_unique<Profile>(id);
    return *m_profiles[id];
  }
}

std::vector<Profile*>
ProfileManager::get_profiles()
{
  std::vector<int> ids;
  physfsutil::enumerate_files("/", [&ids](const std::string& filename) {
      if (filename.substr(0, 7) == "profile")
        ids.push_back(std::stoi(filename.substr(7)));
      return false;
    });
  std::sort(ids.begin(), ids.end());

  std::vector<Profile*> result;
  for (const int& id : ids)
    result.push_back(&get_profile(id));

  return result;
}

void
ProfileManager::reset_profile(int id)
{
  physfsutil::remove_content("profile" + std::to_string(id));

  get_profile(id).reset();
}

void
ProfileManager::delete_profile(int id)
{
  physfsutil::remove_with_content("profile" + std::to_string(id));

  auto it = m_profiles.find(id);
  if (it != m_profiles.end())
    m_profiles.erase(it);
}

/* EOF */
