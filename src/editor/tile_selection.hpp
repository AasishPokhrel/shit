//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_EDITOR_TILE_SELECTION_HPP
#define HEADER_SUPERTUX_EDITOR_TILE_SELECTION_HPP

#include <algorithm>
#include <stdint.h>
#include <vector>

class TileSelection final
{
public:
  TileSelection();

  /** Returns the tile id at specific coordinates. The coordinates
      might be bigger than the selection size or negative. */
  uint32_t pos(int x, int y);

  /** Sets the tile selection to a single tile. */
  void set_tile(uint32_t tile);

  /** Clears the selection. */
  void clear();

  /** Returns true when has no tiles or is full of zeros */
  bool empty() const;

public:
  std::vector<uint32_t> m_tiles;
  int m_width;
  int m_height;

private:
  TileSelection(const TileSelection&) = delete;
  TileSelection& operator=(const TileSelection&) = delete;
};

#endif

/* EOF */
