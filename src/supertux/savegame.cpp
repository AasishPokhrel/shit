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

#include "supertux/savegame.hpp"

#include <algorithm>
#include <physfs.h>

#include "control/input_manager.hpp"
#include "physfs/physfs_file_system.hpp"
#include "physfs/util.hpp"
#include "squirrel/serialize.hpp"
#include "squirrel/squirrel_virtual_machine.hpp"
#include "supertux/player_status.hpp"
#include "supertux/profile_manager.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "worldmap/worldmap.hpp"

namespace {

std::vector<LevelState> get_level_states(ssq::Table& levels)
{
  std::vector<LevelState> results;
  for (const auto& [key, value] : levels.convertRaw())
  {
    try
    {
      ssq::Table table = value.toTable();

      LevelState level_state;
      level_state.filename = key;
      table.get("solved", level_state.solved);
      table.get("perfect", level_state.perfect);

      results.push_back(std::move(level_state));
    }
    catch (const ssq::TypeException&)
    {
      // Ignore non-table entries
    }
  }

  return results;
}

} // namespace

void
LevelsetState::store_level_state(const LevelState& in_state)
{
  auto it = std::find_if(level_states.begin(), level_states.end(),
                         [&in_state](const LevelState& state)
                         {
                           return state.filename == in_state.filename;
                         });
  if (it != level_states.end())
  {
    *it = in_state;
  }
  else
  {
    level_states.push_back(in_state);
  }
}

LevelState
LevelsetState::get_level_state(const std::string& filename) const
{
  auto it = std::find_if(level_states.begin(), level_states.end(),
                         [filename](const LevelState& state)
                         {
                           return state.filename == filename;
                         });
  if (it != level_states.end())
  {
    return *it;
  }
  else
  {
    log_info << "creating new level state for " << filename << std::endl;
    LevelState state;
    state.filename = filename;
    return state;
  }
}


std::unique_ptr<Savegame>
Savegame::from_profile(int profile, const std::string& world_name, bool base_data)
{
  auto savegame = std::make_unique<Savegame>(ProfileManager::current()->get_profile(profile), world_name);
  savegame->load(base_data);
  return savegame;
}

std::unique_ptr<Savegame>
Savegame::from_current_profile(const std::string& world_name, bool base_data)
{
  auto savegame = std::make_unique<Savegame>(ProfileManager::current()->get_current_profile(), world_name);
  savegame->load(base_data);
  return savegame;
}


Savegame::Savegame(Profile& profile, const std::string& world_name) :
  m_profile(profile),
  m_world_name(world_name),
  m_player_status(new PlayerStatus(InputManager::current()->get_num_users())),
  m_state_table()
{
}

std::string
Savegame::get_filename() const
{
  return FileSystem::join(m_profile.get_basedir(), m_world_name + ".stsg");
}

bool
Savegame::is_title_screen() const
{
  // bit of a hack, TitleScreen uses a dummy savegame without a world name
  return m_world_name.empty();
}

void
Savegame::load(bool base_data)
{
  if (m_world_name.empty())
  {
    log_debug << "no world name provided for savegame, skipping load" << std::endl;
    return;
  }

  clear_state_table();

  const std::string filename = get_filename();

  if (!PHYSFS_exists(filename.c_str()))
  {
    log_info << filename << " doesn't exist, not loading state" << std::endl;
  }
  else
  {
    if (physfsutil::is_directory(filename))
    {
      log_info << filename << " is a directory, not loading state" << std::endl;
      return;
    }
    log_debug << "loading savegame from " << filename << std::endl;

    try
    {
      auto doc = ReaderDocument::from_file(filename);
      auto root = doc.get_root();

      if (root.get_name() != "supertux-savegame")
      {
        throw std::runtime_error("file is not a supertux-savegame file");
      }
      else
      {
        auto mapping = root.get_mapping();

        int version = 1;
        mapping.get("version", version);
        if (version != 1)
        {
          throw std::runtime_error("incompatible savegame version");
        }
        else
        {
          /** Load Tux */
          std::optional<ReaderMapping> tux;
          if (!mapping.get("tux", tux))
          {
            throw std::runtime_error("No tux section in savegame");
          }
          {
            m_player_status->read(*tux);
          }

          if (base_data)
            return;

          /** Load "state" table */
          std::optional<ReaderMapping> state;
          if (!mapping.get("state", state))
            throw std::runtime_error("No state section in savegame");
          else
            load_squirrel_table(m_state_table, *state);
        }
      }
    }
    catch(const std::exception& e)
    {
      log_fatal << "Couldn't load savegame: " << e.what() << std::endl;
    }
  }
}

void
Savegame::clear_state_table()
{
  ssq::VM& vm = SquirrelVirtualMachine::current()->get_vm();

  vm.remove("state");
  m_state_table = vm.addTable("state");
}

void
Savegame::save()
{
  if (m_world_name.empty())
  {
    log_debug << "no world name set for savegame, skipping save" << std::endl;
    return;
  }

  const std::string filename = get_filename();
  log_debug << "saving savegame to " << filename << std::endl;

  m_profile.save(); // Make sure profile directory exists, save profile info

  Writer writer(filename);

  writer.start_list("supertux-savegame");
  writer.write("version", 1);

  using namespace worldmap;
  if (WorldMap::current() != nullptr)
  {
    std::ostringstream title;
    title << WorldMap::current()->get_title();
    title << " (" << WorldMap::current()->solved_level_count()
          << "/" << WorldMap::current()->level_count() << ")";
    writer.write("title", title.str());
  }

  writer.start_list("tux");
  m_player_status->write(writer);
  writer.end_list("tux");

  writer.start_list("state");
  try
  {
    save_squirrel_table(m_state_table, writer);
  }
  catch(const std::exception&)
  {
  }
  writer.end_list("state");

  writer.end_list("supertux-savegame");
}

std::vector<std::string>
Savegame::get_worldmaps()
{
  std::vector<std::string> worlds;

  try
  {
    ssq::Table worlds_table = m_state_table.getOrCreateTable("worlds");
    worlds = worlds_table.getKeys();
  }
  catch(const std::exception& err)
  {
    log_warning << err.what() << std::endl;
  }

  // ensure that the loaded worldmap names have their canonical form
  std::transform(worlds.begin(), worlds.end(), worlds.begin(), physfsutil::realpath);

  return worlds;
}

WorldmapState
Savegame::get_worldmap_state(const std::string& name)
{
  WorldmapState result;

  try
  {
    ssq::Table worlds = m_state_table.getOrCreateTable("worlds");

    // if a non-canonical entry is present, replace them with a canonical one
    if (name != "/levels/world2/worldmap.stwm")
    {
      std::string old_map_filename = name.substr(1);
      if (worlds.hasEntry(old_map_filename.c_str()))
        worlds.rename(old_map_filename.c_str(), name.c_str());
    }

    ssq::Table levels = worlds.getOrCreateTable(name.c_str()).getOrCreateTable("levels");
    result.level_states = get_level_states(levels);
  }
  catch(const std::exception& err)
  {
    log_warning << err.what() << std::endl;
  }

  return result;
}

std::vector<std::string>
Savegame::get_levelsets()
{
  std::vector<std::string> results;

  try
  {
    ssq::Table levelsets = m_state_table.getOrCreateTable("levelsets");
    results = levelsets.getKeys();
  }
  catch(const std::exception& err)
  {
    log_warning << err.what() << std::endl;
  }

  return results;
}

LevelsetState
Savegame::get_levelset_state(const std::string& basedir)
{
  LevelsetState result;

  try
  {
    ssq::Table levelsets = m_state_table.getOrCreateTable("levelsets");
    ssq::Table levels = levelsets.getOrCreateTable(basedir.c_str()).getOrCreateTable("levels");
    result.level_states = get_level_states(levels);
  }
  catch(const std::exception& err)
  {
    log_warning << err.what() << std::endl;
  }

  return result;
}

void
Savegame::set_levelset_state(const std::string& basedir,
                             const std::string& level_filename,
                             bool solved)
{
  LevelsetState state = get_levelset_state(basedir);

  try
  {
    ssq::Table levelsets = m_state_table.getOrCreateTable("levelsets");
    ssq::Table levels = levelsets.getOrCreateTable(basedir.c_str()).getOrCreateTable("levels");
    ssq::Table level = levels.getOrCreateTable(level_filename.c_str());

    bool old_solved = false;
    level.get("solved", old_solved);
    level.set("solved", solved || old_solved);
  }
  catch(const std::exception& err)
  {
    log_warning << err.what() << std::endl;
  }
}

/* EOF */
