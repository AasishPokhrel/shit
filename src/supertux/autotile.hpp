//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_AUTOTILE_HPP
#define HEADER_SUPERTUX_SUPERTUX_AUTOTILE_HPP

#include <algorithm>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

class AutotileMask final
{
public:
  AutotileMask(uint8_t mask, bool center);

  bool matches(uint8_t mask, bool center) const;

  inline uint8_t get_mask() const { return m_mask; }

private:
  uint8_t m_mask;
  bool m_center; // m_center should *always* be the same as the m_solid of the corresponding Autotile
};

class Autotile final
{
public:
  struct AltConditions final
  {
    std::pair<uint32_t, uint32_t> period_x;
    std::pair<uint32_t, uint32_t> period_y;
    float weight;
  };

public:
  Autotile(uint32_t tile_id,
    const std::vector<std::pair<uint32_t, AltConditions>>& alt_tiles,
    const std::vector<AutotileMask>& masks,
    bool solid);

  bool matches(uint8_t mask, bool center) const;

  /** @deprecated Returns the base tile ID. */
  inline uint32_t get_tile_id() const { return m_tile_id; }

  /** Picks a tile randomly amongst the possible ones for this autotile. */
  uint32_t pick_tile(int x, int y) const;

  /** @returns true if the given tile has this tile id */
  bool is_amongst(uint32_t tile) const;

  /** @returns the first accessible mask for that autotile */
  uint8_t get_first_mask() const;

  /** Returns all possible tiles for this autotile */
  inline const std::vector<std::pair<uint32_t, AltConditions>>& get_all_tile_ids() const { return m_alt_tiles; }

  /** Returns true if the "center" bool of masks are true. All masks of given Autotile must have the same value for their "center" property.*/
  inline bool is_solid() const { return m_solid; }

private:
  uint32_t m_tile_id;
  std::vector<std::pair<uint32_t, AltConditions>> m_alt_tiles;
  std::vector<AutotileMask> m_masks;
  bool m_solid;

private:
  Autotile(const Autotile&) = delete;
  Autotile& operator=(const Autotile&) = delete;
};

class AutotileSet final
{
public:
  // Moved to tile_set.hpp
  //static AutotileSet* get_tileset_from_tile(uint32_t tile_id);

public:
  AutotileSet(const std::vector<Autotile*>& autotiles, uint32_t default_tile, const std::string& name, bool corner);
  ~AutotileSet();

  /** Returns the ID of the tile to use, based on the surrounding tiles.
   *  If the autotileset is corner-based, the top, left, right, bottom and
   *  center attributes are ignored.
   */
  uint32_t get_autotile(uint32_t tile_id,
    bool top_left, bool top, bool top_right,
    bool left, bool center, bool right,
    bool bottom_left, bool bottom, bool bottom_right,
    int x, int y
  ) const;

  /** Returns the id of the first block in the autotileset. Used for erronous configs. */
  inline uint32_t get_default_tile() const { return m_default; }

  inline const std::string& get_name() const { return m_name; }

  /** true if the given tile is present in the autotileset */
  bool is_member(uint32_t tile_id) const;

  /** true if is_member() is true AND the "center" bool is true */
  bool is_solid(uint32_t tile_id) const;

  /** true if this is a corner-based autotileset */
  inline bool is_corner() const { return m_corner; }
  
  /** Returns the first mask corresponding to the current tile
   *  (useful for corners-based autotilesets)
   */
  uint8_t get_mask_from_tile(uint32_t tile) const;

  // TODO : Validate autotile config files by checking if each mask has
  //        one and only one corresponding tile.
  void validate(int32_t start, int32_t end) const;

public:
  static std::vector<std::unique_ptr<AutotileSet>> m_autotilesets;

private:
  std::vector<Autotile*> m_autotiles;
  uint32_t m_default;
  std::string m_name;
  bool m_corner;

private:
  AutotileSet(const AutotileSet&) = delete;
  AutotileSet& operator=(const AutotileSet&) = delete;
};

#endif

/* EOF */
