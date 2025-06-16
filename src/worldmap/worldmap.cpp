//  SuperTux -  A Jump'n Run
//  Copyright (C) 2004 Ingo Ruhnke <grumbel@gmail.com>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//                2023 Vankata453
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

#include "worldmap/worldmap.hpp"

#include "audio/sound_manager.hpp"
#include "gui/menu_manager.hpp"
#include "physfs/util.hpp"
#include "supertux/constants.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/level.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/player_status.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/tile_manager.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "video/compositor.hpp"
#include "video/drawing_context.hpp"
#include "worldmap/direction.hpp"
#include "worldmap/level_tile.hpp"
#include "worldmap/tux.hpp"
#include "worldmap/world_select.hpp"
#include "worldmap/worldmap_sector.hpp"
#include "worldmap/worldmap_sector_parser.hpp"
#include "worldmap/worldmap_state.hpp"

namespace worldmap {

WorldMap::WorldMap(const std::string& filename, Savegame& savegame,
                   const std::string& force_sector, const std::string& force_spawnpoint) :
  m_sector(),
  m_sectors(),
  m_force_spawnpoint(force_spawnpoint),
  m_savegame(savegame),
  m_tileset(),
  m_name(),
  m_map_filename(physfsutil::realpath(filename)),
  m_levels_path(FileSystem::dirname(m_map_filename)),
  m_next_worldmap(),
  m_passive_message(),
  m_passive_message_timer(),
  m_allow_item_pocket(true),
  m_enter_level(false),
  m_in_level(false),
  m_in_world_select(false)
{
  SoundManager::current()->preload("sounds/warp.wav");

  /** Parse worldmap */
  register_translation_directory(m_map_filename);
  auto doc = ReaderDocument::from_file(m_map_filename);
  auto root = doc.get_root();

  if (root.get_name() != "supertux-level")
    throw std::runtime_error("file isn't a supertux-level file.");

  auto mapping = root.get_mapping();

  mapping.get("name", m_name);

  std::string tileset_name;
  mapping.get("tileset", tileset_name, "images/ice_world.strf");
  m_tileset = TileManager::current()->get_tileset(tileset_name);

  std::string name = "on";
  mapping.get("allow-item-pocket", name);
  m_allow_item_pocket = (Level::get_setting_from_name(name) == Level::ON);

  auto iter = mapping.get_iter();
  while (iter.next())
  {
    if (iter.get_key() == "sector")
      add_sector(WorldMapSectorParser::from_reader(*this, iter.as_mapping()));
  }

  /** Force the initial sector, if provided */
  if (!force_sector.empty())
    set_sector(force_sector, "", false);
}


void
WorldMap::setup()
{
  MenuManager::instance().clear_menu_stack();

  load_state();
  m_sector->setup();

  /** Force the initial spawnpoint, if provided */
  if (!m_force_spawnpoint.empty())
  {
    m_sector->move_to_spawnpoint(m_force_spawnpoint);
    m_force_spawnpoint.clear();
  }

  m_in_world_select = false;
}

void
WorldMap::leave()
{
  save_state();
  m_sector->leave();
}


void
WorldMap::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();
  m_sector->draw(context);
}

void
WorldMap::update(float dt_sec, const Controller& controller)
{
  if (m_in_world_select) return;

  process_input(controller);

  if (m_in_level) return;
  if (MenuManager::instance().is_active()) return;

  if (m_next_worldmap) // A worldmap is scheduled to be changed to.
  {
    m_savegame.get_player_status().last_worldmap = m_next_worldmap->m_map_filename;
    ScreenManager::current()->pop_screen();
    ScreenManager::current()->push_screen(std::move(m_next_worldmap));
    return;
  }

  m_sector->update(dt_sec);
}

void
WorldMap::process_input(const Controller& controller)
{
  m_enter_level = false;

  if (controller.pressed(Control::ACTION) && !m_in_level)
  {
    MenuManager::instance().clear_menu_stack();
    ScreenManager::current()->push_screen(std::make_unique<WorldSelect>(m_map_filename),
          std::make_unique<FadeToBlack>(FadeToBlack::Direction::FADEOUT, .25f));
    m_in_world_select = true;
    return;
  }

  if (controller.pressed_any(Control::JUMP, Control::MENU_SELECT))
  {
    // some people define UP and JUMP on the same key...
    if (!controller.pressed(Control::UP)) {
      m_enter_level = true;
    }
  }

  if (controller.pressed_any(Control::START, Control::ESCAPE))
  {
    on_escape_press();
  }

  if (controller.pressed(Control::CHEAT_MENU) &&
      g_config->developer_mode)
  {
    MenuManager::instance().set_menu(MenuStorage::WORLDMAP_CHEAT_MENU);
  }

  if (controller.pressed(Control::DEBUG_MENU) &&
      g_config->developer_mode)
  {
    MenuManager::instance().set_menu(MenuStorage::DEBUG_MENU);
  }
}

IntegrationStatus
WorldMap::get_status() const
{
  IntegrationStatus status;
  status.m_details.push_back("In worldmap");
  status.m_details.push_back(m_name);
  return status;
}


void
WorldMap::on_escape_press()
{
  // Show or hide the menu
  if (!MenuManager::instance().is_active())
  {
    MenuManager::instance().set_menu(MenuStorage::WORLDMAP_MENU);
    m_sector->get_tux().set_direction(Direction::NONE); // stop tux movement when menu is called
  }
}


size_t
WorldMap::level_count() const
{
  size_t count = 0;
  for (auto& sector : m_sectors)
  {
    count += sector->level_count();
  }
  return count;
}

size_t
WorldMap::solved_level_count() const
{
  size_t count = 0;
  for (auto& sector : m_sectors)
  {
    count += sector->solved_level_count();
  }
  return count;
}


void
WorldMap::load_state()
{
  WorldMapState state(*this);
  state.load_state();
}

void
WorldMap::save_state()
{
  WorldMapState state(*this);
  state.save_state();
}


void
WorldMap::change(const std::string& filename, const std::string& force_sector,
                 const std::string& force_spawnpoint)
{
  // Schedule worldmap to be changed to next frame.
  m_next_worldmap = std::make_unique<WorldMap>(filename, m_savegame, force_sector, force_spawnpoint);
}


void
WorldMap::set_levels_solved(bool solved, bool perfect)
{
  for (auto& level : m_sector->get_objects_by_type<LevelTile>())
  {
    level.set_solved(solved);
    level.set_perfect(perfect);
  }
}


WorldMapSector*
WorldMap::get_sector(const std::string& name) const
{
  for (auto& sector : m_sectors)
  {
    if (sector->get_name() == name)
      return sector.get();
  }
  return nullptr;
}

WorldMapSector*
WorldMap::get_sector(int index) const
{
  if (index < 0 || index > static_cast<int>(m_sectors.size()) - 1)
    return nullptr;

  return m_sectors.at(index).get();
}


void
WorldMap::add_sector(std::unique_ptr<WorldMapSector> sector)
{
  m_sectors.push_back(std::move(sector));
}

void
WorldMap::set_sector(const std::string& name, const std::string& spawnpoint,
                     bool perform_full_setup)
{
  if (m_sector) // There is a current sector.
  {
    save_state();
    m_sector->leave();
  }

  m_sector = get_sector(name);

  if (!m_sector) // The sector was not found, so no sector is assigned.
  {
    log_warning << "Sector '" << name << "' not found. Setting first sector." << std::endl;
    m_sector = get_sector(0); // In that case, assign the first sector.
  }

  m_sector->move_to_spawnpoint(DEFAULT_SPAWNPOINT_NAME);

  // Set up the new sector.
  if (perform_full_setup)
    load_state();
  m_sector->setup();

  // If a spawnpoint has been provided, move to it.
  if (!spawnpoint.empty())
    m_sector->move_to_spawnpoint(spawnpoint);
}

const std::string&
WorldMap::get_filename() const
{
  return m_map_filename;
}

} // namespace worldmap

/* EOF */
