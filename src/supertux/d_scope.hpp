//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_D_SCOPE_HPP
#define HEADER_SUPERTUX_SUPERTUX_D_SCOPE_HPP

#include "util/dynamic_scoped_ref.hpp"

class Sector;
extern DynamicScopedRef<Sector> d_sector;

namespace worldmap {
class WorldMapSector;
} // namespace worldmap

extern DynamicScopedRef<worldmap::WorldMapSector> d_worldmap_sector;

class GameObjectManager;
extern DynamicScopedRef<GameObjectManager> d_gameobject_manager;

#define BIND_SECTOR(x) \
  auto sector_guard = d_sector.bind(x);  \
  auto gameobject_manager_guard = d_gameobject_manager.bind(x)

#define BIND_WORLDMAP_SECTOR(x) \
  auto worldmap_guard = d_worldmap_sector.bind(x);  \
  auto gameobject_manager_guard = d_gameobject_manager.bind(x)

#endif

/* EOF */
