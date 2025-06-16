//  SuperTux
//  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/game_manager.hpp"

#include "editor/editor.hpp"
#include "sdk/integration.hpp"
#include "supertux/levelset_screen.hpp"
#include "supertux/player_status.hpp"
#include "supertux/profile.hpp"
#include "supertux/savegame.hpp"
#include "supertux/screen.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/world.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "worldmap/tux.hpp"
#include "worldmap/worldmap.hpp"

GameManager::GameManager() :
  m_savegame()
{
}

void
GameManager::start_level(const World& world, const std::string& level_filename,
                         const std::optional<std::pair<std::string, Vector>>& start_pos)
{
  m_savegame = Savegame::from_current_profile(world.get_basename());

  auto screen = std::make_unique<LevelsetScreen>(world.get_basedir(),
                                                 level_filename,
                                                 *m_savegame,
                                                 start_pos);
  ScreenManager::current()->push_screen(std::move(screen));

  if (!Editor::current())
    m_savegame->get_profile().set_last_world(world.get_basename());
}

bool
GameManager::start_worldmap(const World& world, const std::string& worldmap_filename,
                            const std::string& sector, const std::string& spawnpoint)
{
  try
  {
    m_savegame = Savegame::from_current_profile(world.get_basename());

    auto filename = m_savegame->get_player_status().last_worldmap;
    // If we specified a worldmap filename manually,
    // this overrides the default choice of "last worldmap".
    if (!worldmap_filename.empty())
    {
      filename = worldmap_filename;
    }

    // No "last worldmap" found and no worldmap_filename
    // specified. Let's go ahead and use the worldmap
    // filename specified in the world.
    if (filename.empty())
    {
      filename = world.get_worldmap_filename();
    }

    auto worldmap = std::make_unique<worldmap::WorldMap>(filename, *m_savegame, sector, spawnpoint);
    ScreenManager::current()->push_screen(std::move(worldmap));

    if (!Editor::current())
      m_savegame->get_profile().set_last_world(world.get_basename());
  }
  catch (const std::exception& e)
  {
    log_warning << "Couldn't start worldmap: " << e.what() << std::endl;
    return false;
  }
  return true;
}

bool
GameManager::start_worldmap(const World& world, const std::string& worldmap_filename,
                            const std::optional<std::pair<std::string, Vector>>& start_pos)
{
  if (!start_worldmap(world, worldmap_filename, start_pos ? start_pos->first : ""))
    return false;

  if (start_pos)
    worldmap::WorldMapSector::current()->get_tux().set_initial_pos(start_pos->second);

  return true;
}

/* EOF */
