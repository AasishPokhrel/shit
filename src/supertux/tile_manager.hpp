//  SuperTux
//  Copyright (C) 2008 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_TILE_MANAGER_HPP
#define HEADER_SUPERTUX_SUPERTUX_TILE_MANAGER_HPP

#include <map>
#include <memory>
#include <string>

#include "util/currenton.hpp"

class TileSet;

class TileManager final : public Currenton<TileManager>
{
private:
  std::map<std::string, std::unique_ptr<TileSet>> m_tilesets;

public:
  TileManager();

  TileSet* get_tileset(const std::string &filename);

  void reload();

private:
  TileManager(const TileManager&) = delete;
  TileManager& operator=(const TileManager&) = delete;
};

#endif

/* EOF */
