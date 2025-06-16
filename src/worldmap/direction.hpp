//  SuperTux - Worldmap Direction
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_WORLDMAP_DIRECTION_HPP
#define HEADER_SUPERTUX_WORLDMAP_DIRECTION_HPP

#include <string>

namespace worldmap {

enum class Direction { NONE, WEST, EAST, NORTH, SOUTH };

Direction reverse_dir(Direction direction);
Direction string_to_direction(const std::string& directory);
std::string direction_to_string(Direction direction);

} // namespace worldmap

#endif

/* EOF */
