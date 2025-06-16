//  SuperTux
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

#ifndef HEADER_SUPERTUX_WORLDMAP_WORLDMAP_SECTOR_HPP
#define HEADER_SUPERTUX_WORLDMAP_WORLDMAP_SECTOR_HPP

#include "supertux/sector_base.hpp"

#include "worldmap/tux.hpp"

namespace worldmap {

class Camera;
class SpawnPoint;
class WorldMap;

/** Represents one of (potentially) multiple, separate parts of a WorldMap.
    WorldMap variant of Sector, utilizing only its base features. */
/**
 * @scripting
 * @summary This class provides additional controlling functions for a worldmap sector, other than the ones listed at ${SRG_REF_GameObjectManager}.
 * @instances An instance under ""worldmap.settings"" is available from scripts and the console.
 */
class WorldMapSector final : public Base::Sector
{
  friend class WorldMapSectorParser;
  friend class WorldMapState;

public:
  static void register_class(ssq::VM& vm);

public:
  static WorldMapSector* current();

public:
  WorldMapSector(WorldMap& parent);
  ~WorldMapSector() override;

  void finish_construction(bool editable) override;

  std::string get_exposed_class_name() const override { return "WorldMapSector"; }

  void setup();
  void leave();

  void draw(DrawingContext& context) override;
  void update(float dt_sec) override;

  Vector get_next_tile(const Vector& pos, const Direction& direction) const;

  /** gets a bitfield of Tile::WORLDMAP_NORTH | Tile::WORLDMAP_WEST |
      ... values, which indicates the directions Tux can move to when
      at the given position. */
  int available_directions_at(const Vector& pos) const;

  /** returns a bitfield representing the union of all
      Tile::WORLDMAP_XXX values of all solid tiles at the given
      position */
  int tile_data_at(const Vector& pos) const;

  /**
   * returns true if the specified position contains a valid path.
   * @param pos Position to check
   */
  bool is_valid_path_at(const Vector& pos) const;

  size_t level_count() const;
  size_t solved_level_count() const;

  /** gets called from the GameSession when a level has been successfully
      finished */
  void finished_level(Level* level);

  /** Get a spawnpoint by its name @param name The name of the
      spawnpoint @return spawnpoint corresponding to that name */
  SpawnPoint* get_spawnpoint_by_name(const std::string& spawnpoint_name) const;

  template<class T>
  T* at_object() const
  {
    return at_object<T>(m_tux->get_tile_pos());
  }
  template<class T>
  T* at_object(const Vector& pos) const
  {
    for (auto& obj : get_objects_by_type<T>())
      if (obj.get_tile_pos() == pos)
        return &obj;

    return nullptr;
  }

  /** Check if it is possible to walk from \a pos into \a direction,
      if possible, write the new position to \a new_pos */
  bool path_ok(const Direction& direction, const Vector& old_pos, Vector* new_pos) const;

  /** Sets the name of the tilemap that should fade when worldmap is set up. */
  void set_initial_fade_tilemap(const std::string& tilemap_name, int direction);

  bool in_worldmap() const override { return true; }

  /**
   * @scripting
   * @description Returns Tux's X position on the worldmap.
   */
  float get_tux_x() const;
  /**
   * @scripting
   * @description Returns Tux's Y position on the worldmap.
   */
  float get_tux_y() const;

  /**
   * @scripting
   * @description Changes the current sector of the worldmap to a specified new sector.
   * @param string $sector
   */
  void set_sector(const std::string& sector);
  /**
   * @scripting
   * @description Changes the current sector of the worldmap to a specified new sector,
                  moving Tux to the specified spawnpoint.
   * @param string $sector
   * @param string $spawnpoint
   */
  void spawn(const std::string& sector, const std::string& spawnpoint);
  /**
   * @scripting
   * @description Moves Tux to the specified spawnpoint.
   * @param string $spawnpoint
   */
  void move_to_spawnpoint(const std::string& spawnpoint);
  void move_to_spawnpoint(const std::string& spawnpoint, bool pan);

  /**
   * @scripting
   * @description Gets the path to the worldmap file. Useful for saving worldmap-specific data.
   */
  std::string get_filename() const;
  /**
   * @scripting
   * @description Overrides the "Title Screen Level" property for the world with ""filename"".
                  The newly set level will be used for the title screen, after exiting the world.
   * @param string $filename
   */
  void set_title_level(const std::string& filename);

  TileSet* get_tileset() const override;
  inline WorldMap& get_worldmap() const { return m_parent; }
  inline Camera& get_camera() const { return *m_camera; }
  inline Tux& get_tux() const { return *m_tux; }
  Vector get_tux_pos() const;

protected:
  MovingObject& add_object_scripting(const std::string& class_name, const std::string& name,
                                     const Vector& pos, const std::string& direction,
                                     const std::string& data) override;

  void draw_status(DrawingContext& context);

private:
  WorldMap& m_parent;

  std::unique_ptr<Camera> m_camera;
  Tux* m_tux;
  std::vector<std::unique_ptr<SpawnPoint> > m_spawnpoints;

  std::string m_initial_fade_tilemap;
  int m_fade_direction;

private:
  WorldMapSector(const WorldMapSector&) = delete;
  WorldMapSector& operator=(const WorldMapSector&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
