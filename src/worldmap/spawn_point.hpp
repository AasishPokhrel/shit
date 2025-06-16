//  SuperTux - Worldmap Spawnpoint
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_WORLDMAP_SPAWN_POINT_HPP
#define HEADER_SUPERTUX_WORLDMAP_SPAWN_POINT_HPP

#include "worldmap/worldmap_object.hpp"

#include <string>

#include "math/vector.hpp"
#include "worldmap/direction.hpp"

namespace worldmap {

class SpawnPoint final
{
public:
  SpawnPoint(const ReaderMapping& mapping);

  inline const std::string& get_name() const { return m_name; }
  inline Vector get_pos() const { return m_pos; }
  inline Direction get_auto_dir() const { return m_auto_dir; }

private:
  std::string m_name;
  Vector m_pos;
  Direction m_auto_dir; /**< automatically start walking in this direction */

private:
  SpawnPoint(const SpawnPoint&) = delete;
  SpawnPoint& operator=(const SpawnPoint&) = delete;
};


class SpawnPointObject final : public WorldMapObject
{
public:
  SpawnPointObject(const ReaderMapping& mapping);
  SpawnPointObject(const std::string& name, const Vector& pos);

  static std::string class_name() { return "worldmap-spawnpoint"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Spawn point"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WorldMapObject::get_class_types().add(typeid(SpawnPointObject)); }

  virtual ObjectSettings get_settings() override;

private:
  Direction m_dir;

private:
  SpawnPointObject(const SpawnPointObject&) = delete;
  SpawnPointObject& operator=(const SpawnPointObject&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
