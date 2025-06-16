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

#ifndef HEADER_SUPERTUX_SUPERTUX_TILE_SET_HPP
#define HEADER_SUPERTUX_SUPERTUX_TILE_SET_HPP

#include <map>
#include <memory>
#include <stdint.h>
#include <string>

#include "math/fwd.hpp"
#include "supertux/autotile.hpp"
#include "supertux/tile.hpp"
#include "video/color.hpp"
#include "video/surface_ptr.hpp"

class Canvas;
class DrawingContext;

class Tilegroup final
{
public:
  Tilegroup();

  bool developers_group = false;
  std::string name;
  std::vector<int> tiles;
};

class TileSet final
{
public:
  static std::unique_ptr<TileSet> from_file(const std::string& filename);

public:
  TileSet(const std::string& filename);
  ~TileSet() = default;

  void reload();

  void add_tile(int id, std::unique_ptr<Tile> tile);

  /** Adds a group of tiles that haven't
      been assigned to any other group */
  void add_unassigned_tilegroup();

  /** Check for and remove any deprecated tiles from tilegroups. */
  void remove_deprecated_tiles();

  void add_tilegroup(const Tilegroup& tilegroup);

  const Tile& get(const uint32_t id) const;
  
  std::vector<AutotileSet*> get_autotilesets_from_tile(uint32_t tile_id) const;
  bool has_mutual_autotileset(uint32_t lhs, uint32_t rhs) const;

  uint32_t get_max_tileid() const {
    return static_cast<uint32_t>(m_tiles.size());
  }

  const std::vector<Tilegroup>& get_tilegroups() const {
    return m_tilegroups;
  }

  void print_debug_info();
  
private:
  const std::string m_filename;

public:
  // Must be public because of tile_set_parser.cpp
  std::vector<std::unique_ptr<AutotileSet>> m_autotilesets;

  // Additional attributes

  // Must be public because of tile_set_parser.cpp and thunderstorm.cpp
  std::map<uint32_t, uint32_t> m_thunderstorm_tiles;

private:
  std::vector<std::unique_ptr<Tile> > m_tiles;
  std::vector<Tilegroup> m_tilegroups;

private:
  TileSet(const TileSet&) = delete;
  TileSet& operator=(const TileSet&) = delete;
};

#endif

/* EOF */
