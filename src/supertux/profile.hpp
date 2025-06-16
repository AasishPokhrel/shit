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

#ifndef HEADER_SUPERTUX_SUPERTUX_PROFILE_HPP
#define HEADER_SUPERTUX_SUPERTUX_PROFILE_HPP

#include <memory>
#include <string>

class ReaderMapping;

/** Contains general data about a profile, which preserves savegames. */
class Profile final
{
public:
  Profile(int id);

public:
  void save();
  void reset();

  void create_basedir();
  std::string get_basedir() const;

  inline int get_id() const { return m_id; }
  inline const std::string& get_name() const { return m_name; }
  inline const std::string& get_last_world() const { return m_last_world; }

  inline void set_name(const std::string& name) { m_name = name; }
  inline void set_last_world(const std::string& world) { m_last_world = world; }

private:
  const int m_id;

  std::string m_name;
  std::string m_last_world;

private:
  Profile(const Profile&) = delete;
  Profile& operator=(const Profile&) = delete;
};

#endif

/* EOF */
