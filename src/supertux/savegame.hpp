//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_SAVEGAME_HPP
#define HEADER_SUPERTUX_SUPERTUX_SAVEGAME_HPP

#include <memory>
#include <string>
#include <vector>

#include <simplesquirrel/table.hpp>

class PlayerStatus;
class Profile;

struct LevelState
{
public:
  LevelState() :
    filename(),
    solved(false),
    perfect(false)
  {}

  std::string filename;
  bool solved;
  bool perfect;
};

struct LevelsetState
{
public:
  LevelsetState() :
    directory(),
    level_states()
  {}
  std::string directory;
  std::vector<LevelState> level_states;

  LevelState get_level_state(const std::string& filename) const;
  void store_level_state(const LevelState& state);
};

struct WorldmapState
{
public:
  WorldmapState() :
    filename(),
    level_states()
  {}
  std::string filename;
  std::vector<LevelState> level_states;
};

class Savegame final
{
public:
  static std::unique_ptr<Savegame> from_profile(int profile, const std::string& world_name, bool base_data = false);
  static std::unique_ptr<Savegame> from_current_profile(const std::string& world_name, bool base_data = false);

public:
  Savegame(Profile& profile, const std::string& world_name);

  inline Profile& get_profile() const { return m_profile; }
  std::string get_filename() const;

  /** Returns content of (tux ...) entry */
  inline PlayerStatus& get_player_status() const { return *m_player_status; }

  std::string get_title() const;

  std::vector<std::string> get_levelsets();
  LevelsetState get_levelset_state(const std::string& name);
  void set_levelset_state(const std::string& basedir,
                          const std::string& level_filename,
                          bool solved);

  std::vector<std::string> get_worldmaps();
  WorldmapState get_worldmap_state(const std::string& name);

  void save();

  bool is_title_screen() const;

private:
  void load(bool base_data = false);
  void clear_state_table();

private:
  Profile& m_profile;
  std::string m_world_name;
  std::unique_ptr<PlayerStatus> m_player_status;
  ssq::Table m_state_table;

private:
  Savegame(const Savegame&) = delete;
  Savegame& operator=(const Savegame&) = delete;
};

#endif

/* EOF */
