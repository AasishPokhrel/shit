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

#include "editor/tile_selection.hpp"

TileSelection::TileSelection() :
  m_tiles(1),
  m_width(1),
  m_height(1)
{
}

uint32_t
TileSelection::pos(int x, int y)
{
  if (m_width < 1)
    m_width = 1;

  if(m_height < 1)
    m_height = 1;

  x = x % m_width;
  y = y % m_height;
  if (x < 0) {
    x += m_width;
  }
  if (y < 0) {
    y += m_height;
  }

  return m_tiles[x + y * m_width];
}

void
TileSelection::set_tile(uint32_t tile)
{
  m_tiles.clear();
  m_width = 1;
  m_height = 1;
  m_tiles.push_back(tile);
}

void
TileSelection::clear()
{
  m_tiles.clear();
  m_width = 0;
  m_height = 0;
}

bool
TileSelection::empty() const
{
  return std::all_of(m_tiles.begin(), m_tiles.end(), [](const auto& tile) {
    return tile == 0;
  });
}

/* EOF */
