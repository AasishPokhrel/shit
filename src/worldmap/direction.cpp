//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "worldmap/direction.hpp"

#include "util/log.hpp"

namespace worldmap {

Direction reverse_dir(Direction direction)
{
  switch (direction)
  {
    case Direction::WEST:
      return Direction::EAST;
    case Direction::EAST:
      return Direction::WEST;
    case Direction::NORTH:
      return Direction::SOUTH;
    case Direction::SOUTH:
      return Direction::NORTH;
    case Direction::NONE:
      return Direction::NONE;
  }
  return Direction::NONE;
}

std::string direction_to_string(Direction direction)
{
  switch (direction)
  {
    case Direction::WEST:
      return "west";
    case Direction::EAST:
      return "east";
    case Direction::NORTH:
      return "north";
    case Direction::SOUTH:
      return "south";
    default:
      return "none";
  }
}

Direction string_to_direction(const std::string& directory)
{
  if (directory == "west")
    return Direction::WEST;
  else if (directory == "east")
    return Direction::EAST;
  else if (directory == "north")
    return Direction::NORTH;
  else if (directory == "south")
    return Direction::SOUTH;
  else if (directory == "none")
    return Direction::NONE;
  else {
    log_warning << "Unknown direction: \"" << directory << "\"" << std::endl;
    return Direction::NONE;
  }
}

} // namespace worldmap

/* EOF */
