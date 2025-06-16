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

#ifndef HEADER_SUPERTUX_WORLDMAP_WORLDMAP_HPP
#define HEADER_SUPERTUX_WORLDMAP_WORLDMAP_HPP

#include "supertux/screen.hpp"
#include "util/currenton.hpp"

#include "control/controller.hpp"
#include "supertux/savegame.hpp"
#include "supertux/timer.hpp"
#include "worldmap/worldmap_sector.hpp"

class TileSet;

namespace worldmap {

class WorldMap final : public Screen,
                       public Currenton<WorldMap>
{
  friend class WorldMapSector;
  friend class WorldMapState;

public:
  static Color s_level_title_color;
  static Color s_message_color;
  static Color s_teleporter_message_color;

public:
  WorldMap(const std::string& filename, Savegame& savegame,
           const std::string& force_sector = "", const std::string& force_spawnpoint = "");

  void setup() override;
  void leave() override;

  void draw(Compositor& compositor) override;
  void update(float dt_sec, const Controller& controller) override;

  IntegrationStatus get_status() const override;

  size_t level_count() const;
  size_t solved_level_count() const;

  /** Load worldmap state from squirrel state table */
  void load_state();

  /** Save worldmap state to squirrel state table */
  void save_state();

  /** switch to another worldmap.
      filename is relative to data root path */
  void change(const std::string& filename, const std::string& force_sector = "",
              const std::string& force_spawnpoint = "");

  /** Mark all levels as solved or unsolved */
  void set_levels_solved(bool solved, bool perfect);

  /** Sets the passive message with specific time **/
  inline void set_passive_message(const std::string& message, float time)
  {
    m_passive_message = message;
    m_passive_message_timer.start(time);
  }

  /** Sets the initial spawnpoint to be forced on next setup */
  inline void set_initial_spawnpoint(const std::string& spawnpoint) { m_force_spawnpoint = spawnpoint; }

  inline const std::string& get_title() const { return m_name; }
  inline Savegame& get_savegame() const { return m_savegame; }
  inline const std::string& get_levels_path() const { return m_levels_path; }

  WorldMapSector* get_sector(const std::string& name) const;
  WorldMapSector* get_sector(int index) const;

  void add_sector(std::unique_ptr<WorldMapSector> sector);
  inline WorldMapSector& get_sector() const { return *m_sector; }
  void set_sector(const std::string& name, const std::string& spawnpoint = "",
                  bool perform_full_setup = true);

  const std::string& get_filename() const;

  bool is_item_pocket_allowed() const { return m_allow_item_pocket; }

private:
  void process_input(const Controller& controller);

  void on_escape_press();

private:
  WorldMapSector* m_sector; /* The currently active sector. */
  std::vector<std::unique_ptr<WorldMapSector> > m_sectors;

  std::string m_force_spawnpoint;

  Savegame& m_savegame;
  TileSet* m_tileset;

  std::string m_name;
  std::string m_map_filename;
  std::string m_levels_path;

  /* A worldmap, scheduled to change to next frame. */
  std::unique_ptr<WorldMap> m_next_worldmap;

  /** Passive map message variables */
  std::string m_passive_message;
  Timer m_passive_message_timer;

  bool m_allow_item_pocket;
  bool m_enter_level;
  bool m_in_level;
  bool m_in_world_select;

private:
  WorldMap(const WorldMap&) = delete;
  WorldMap& operator=(const WorldMap&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
