//  SuperTux
//  Copyright (C) 2004-2018 Ingo Ruhnke <grumbel@gmail.com>
//                2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "worldmap/worldmap_state.hpp"

#include "math/vector.hpp"
#include "object/music_object.hpp"
#include "object/tilemap.hpp"
#include "squirrel/squirrel_virtual_machine.hpp"
#include "squirrel/squirrel_util.hpp"
#include "supertux/constants.hpp"
#include "supertux/savegame.hpp"
#include "supertux/tile.hpp"
#include "util/log.hpp"
#include "worldmap/direction.hpp"
#include "worldmap/level_tile.hpp"
#include "worldmap/sprite_change.hpp"
#include "worldmap/tux.hpp"
#include "worldmap/worldmap.hpp"

namespace worldmap {

WorldMapState::WorldMapState(WorldMap& worldmap) :
  m_worldmap(worldmap),
  m_position_was_reset(false)
{
}


void
WorldMapState::load_state()
{
  log_debug << "loading worldmap state" << std::endl;

  ssq::VM& vm = SquirrelVirtualMachine::current()->get_vm();
  try
  {
    /** Get state table for all worldmaps. **/
    ssq::Table worlds_table = vm.findTable("state").findTable("worlds");

    // If a non-canonical entry is present, replace it with a canonical one.
    const std::string old_map_filename = m_worldmap.m_map_filename.substr(1);
    if (worlds_table.hasEntry(old_map_filename.c_str()))
      worlds_table.rename(old_map_filename.c_str(), m_worldmap.m_map_filename.c_str());

    /** Get state table for the current worldmap. **/
    ssq::Table worldmap_table = worlds_table.findTable(m_worldmap.m_map_filename.c_str());

    // Load the current sector.
    ssq::Table sector_table;
    if (worldmap_table.hasEntry("sector")) // Load the current sector only if a "sector" property exists.
    {
      const std::string sector_name = worldmap_table.get<std::string>("sector");
      if (!m_worldmap.m_sector) // If the worldmap doesn't have a current sector, try setting the new sector.
        m_worldmap.set_sector(sector_name, "", false);

      /** Get state table for the current sector. **/
      sector_table = worldmap_table.findTable(m_worldmap.get_sector().get_name().c_str());
    }
    else // Sector property does not exist, which may indicate outdated save file.
    {
      if (!m_worldmap.m_sector) // If the worldmap doesn't have a current sector, try setting the main one.
        m_worldmap.set_sector(DEFAULT_SECTOR_NAME, "", false);

      sector_table = worldmap_table;
    }

    if (!m_worldmap.m_sector)
    {
      // Quit loading worldmap state, if there is still no current sector loaded.
      throw std::runtime_error("No sector set.");
    }
    
    try
    {
      ssq::Object music = sector_table.find("music");
      auto& music_object = m_worldmap.get_sector().get_singleton_by_type<MusicObject>();
      music_object.set_music(music.toString());
    }
    catch (const ssq::NotFoundException&)
    {
      log_debug << "Could not find \"music\" in the worldmap sector state table." << std::endl;
    }

    /** Load objects. **/
    load_tux(sector_table);
    load_levels(sector_table);
    load_tilemap_visibility(sector_table);
    load_sprite_change_objects(sector_table);
  }
  catch (const std::exception& err)
  {
    log_warning << "Not loading worldmap state: " << err.what() << std::endl;

    // Set default properties.
    if (!m_worldmap.m_sector)
      m_worldmap.set_sector(DEFAULT_SECTOR_NAME, "", false); // If no current sector is present, set it to "main", or the default one.

    // Create a new initial save.
    save_state();
  }

  m_worldmap.m_in_level = false;
}


/** Load Tux **/
void
WorldMapState::load_tux(const ssq::Table& table)
{
  WorldMapSector& sector = m_worldmap.get_sector();

  const ssq::Table tux = table.findTable("tux");
  Vector p(0.0f, 0.0f);
  if (!tux.get("x", p.x) || !tux.get("y", p.y))
  {
    log_warning << "Player position not set, respawning." << std::endl;
    sector.move_to_spawnpoint(DEFAULT_SPAWNPOINT_NAME);
    m_position_was_reset = true;
  }

  std::string back_str;
  tux.get("back", back_str);
  sector.m_tux->m_back_direction = string_to_direction(back_str);
  sector.m_tux->set_tile_pos(p);

  int tile_data = sector.tile_data_at(p);
  if (!(tile_data & (Tile::WORLDMAP_NORTH | Tile::WORLDMAP_SOUTH | Tile::WORLDMAP_WEST | Tile::WORLDMAP_EAST)))
  {
    log_warning << "Player at illegal position " << p.x << ", " << p.y << " respawning." << std::endl;
    sector.move_to_spawnpoint(DEFAULT_SPAWNPOINT_NAME);
    m_position_was_reset = true;
  }
}

/** Load levels **/
void
WorldMapState::load_levels(const ssq::Table& table)
{
  try
  {
    const ssq::Table levels_table = table.findTable("levels");

    for (auto& level_tile : m_worldmap.get_sector().get_objects_by_type<LevelTile>())
    {
      try
      {
        const ssq::Table level = levels_table.findTable(level_tile.get_level_filename().c_str());

        bool solved = false;
        level.get("solved", solved);
        level_tile.set_solved(solved);

        bool perfect = false;
        level.get("perfect", perfect);
        level_tile.set_perfect(perfect);

        level_tile.update_sprite_action();
        level_tile.get_statistics().unserialize_from_squirrel(level);
      }
      catch (const ssq::NotFoundException&)
      {
        // Level not saved.
      }
    }
  }
  catch (const ssq::NotFoundException&)
  {
    // Level table not saved.
  }
}

/** Load tilemap visibility **/
void
WorldMapState::load_tilemap_visibility(const ssq::Table& table)
{
  if (m_position_was_reset) return;
  WorldMapSector& sector = m_worldmap.get_sector();

  try
  {
    const std::map<std::string, ssq::Object> tilemaps = table.findTable("tilemaps").convertRaw();
    for (const auto& [key, value] : tilemaps)
    {
      TileMap* tilemap = sector.get_object_by_name<TileMap>(key);
      if (tilemap)
      {
        const ssq::Table tilemap_table = value.toTable();

        float alpha = 1.f;
        tilemap_table.get("alpha", alpha);
        tilemap->set_alpha(alpha);
      }
    }
  }
  catch (const ssq::Exception&)
  {
    // Failed to get tilemap entry. This could indicate
    // that no savable tilemaps have been found. In any
    // case: This is not severe at all.
  }
}

/** Load sprite change objects **/
void
WorldMapState::load_sprite_change_objects(const ssq::Table& table)
{
  if (m_worldmap.get_sector().get_object_count<SpriteChange>() <= 0) return;

  const ssq::Table sprite_changes = table.findTable("sprite-changes");
  for (auto& sc : m_worldmap.get_sector().get_objects_by_type<SpriteChange>())
  {
    const std::string key = std::to_string(static_cast<int>(sc.get_pos().x)) + "_" +
                            std::to_string(static_cast<int>(sc.get_pos().y));

    try
    {
      const ssq::Table sprite_change = sprite_changes.findTable(key.c_str());

      bool show_stay_action = false;
      sprite_change.get("show-stay-action", show_stay_action);
      if (show_stay_action)
        sc.set_stay_action();
      else
        sc.clear_stay_action(/* propagate = */ false);
    }
    catch (const ssq::NotFoundException&)
    {
      // Sprite change not saved.
    }
  }
}


void
WorldMapState::save_state() const
{
  WorldMapSector& sector = m_worldmap.get_sector();

  ssq::VM& vm = SquirrelVirtualMachine::current()->get_vm();
  try
  {
    /** Get or create state table for all worldmaps. **/
    ssq::Table worlds_table = vm.findTable("state").getOrCreateTable("worlds");

    /** Get or create state table for the current worldmap. **/
    ssq::Table worldmap_table = worlds_table.getOrCreateTable(m_worldmap.m_map_filename.c_str());

    // Save the current sector.
    worldmap_table.set("sector", sector.get_name());

    /** Delete the table entry for the current sector and construct a new one. **/
    worldmap_table.remove(sector.get_name().c_str());
    ssq::Table sector_table = worldmap_table.addTable(sector.get_name().c_str());

    /** Save Music **/
    auto& music_object = m_worldmap.get_sector().get_singleton_by_type<MusicObject>();
    sector_table.set("music", music_object.get_music());

    /** Save Tux **/
    ssq::Table tux = sector_table.addTable("tux");
    tux.set("x", sector.m_tux->get_tile_pos().x);
    tux.set("y", sector.m_tux->get_tile_pos().y);
    tux.set("back", direction_to_string(sector.m_tux->m_back_direction));

    /** Save levels **/
    ssq::Table levels = sector_table.addTable("levels");
    for (const auto& level_tile : m_worldmap.get_sector().get_objects_by_type<LevelTile>())
    {
      ssq::Table level = levels.addTable(level_tile.get_level_filename().c_str());
      level.set("solved", level_tile.is_solved());
      level.set("perfect", level_tile.is_perfect());
      level_tile.get_statistics().serialize_to_squirrel(level);
    }

    /** Save tilemap visibility **/
    ssq::Table tilemaps = sector_table.addTable("tilemaps");
    for (auto& tilemap : m_worldmap.get_sector().get_objects_by_type<::TileMap>())
    {
      if (!tilemap.get_name().empty())
      {
        ssq::Table tilemap_table = tilemaps.addTable(tilemap.get_name().c_str());
        tilemap_table.set("alpha", tilemap.get_target_alpha());
      }
    }

    /** Save sprite change objects **/
    if (m_worldmap.get_sector().get_object_count<SpriteChange>() > 0)
    {
      ssq::Table sprite_changes = sector_table.addTable("sprite-changes");
      for (const auto& sc : m_worldmap.get_sector().get_objects_by_type<SpriteChange>())
      {
        const std::string key = std::to_string(static_cast<int>(sc.get_pos().x)) + "_" +
                                std::to_string(static_cast<int>(sc.get_pos().y));
        ssq::Table sprite_change = sprite_changes.addTable(key.c_str());
        sprite_change.set("show-stay-action", sc.show_stay_action());
      }
    }
  }
  catch (const std::exception& err)
  {
    log_warning << "Failed to save worldmap state: " << err.what() << std::endl;
  }

  m_worldmap.m_savegame.save();
}

} // namespace worldmap

/* EOF */
