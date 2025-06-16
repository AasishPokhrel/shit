//  SuperTux - Teleporter Worldmap Tile
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

#ifndef HEADER_SUPERTUX_WORLDMAP_TELEPORTER_HPP
#define HEADER_SUPERTUX_WORLDMAP_TELEPORTER_HPP

#include "worldmap/worldmap_object.hpp"

#include <string>

namespace worldmap {

class Teleporter final : public WorldMapObject
{
public:
  Teleporter(const ReaderMapping& mapping);

  static std::string class_name() { return "teleporter"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Teleporter"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WorldMapObject::get_class_types().add(typeid(Teleporter)); }

  virtual ObjectSettings get_settings() override;

  inline const std::string& get_worldmap() const { return m_worldmap; }
  inline const std::string& get_sector() const { return m_sector; }
  inline const std::string& get_spawnpoint() const { return m_spawnpoint; }
  inline bool is_automatic() const { return m_automatic; }
  inline const std::string& get_message() const { return m_message; }

private:
  /** Worldmap filename (relative to data root) to teleport to. Leave empty to use current word */
  std::string m_worldmap;

  /** Sector to teleport to. Leave empty to stay at the current sector **/
  std::string m_sector;

  /** Spawnpoint to teleport to. Leave empty to use "main" or last one */
  std::string m_spawnpoint;

  /** true if this teleporter does not need to be activated, but teleports Tux as soon as it's touched */
  bool m_automatic;

  /** optional map message to display */
  std::string m_message;

private:
  Teleporter(const Teleporter&) = delete;
  Teleporter& operator=(const Teleporter&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
