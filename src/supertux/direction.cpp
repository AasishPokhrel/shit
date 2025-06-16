//  SuperTux
//  Copyright (C) 2008 Ryan Flegel <rflegel@gmail.com>
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

#include <vector>

#include "supertux/direction.hpp"

#include "editor/object_option.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

std::ostream& operator<<(std::ostream& o, const Direction& dir)
{
  return o << dir_to_string(dir);
}

std::string
dir_to_string(const Direction& dir)
{
  switch (dir)
  {
    case Direction::NONE:
      return "none";
    case Direction::LEFT:
      return "left";
    case Direction::RIGHT:
      return "right";
    case Direction::UP:
      return "up";
    case Direction::DOWN:
      return "down";
    default:
      if (dir != Direction::AUTO)
      {
        // Display a warning when an invalid direction has been provided.
        log_warning << "Unknown direction \"" << dir << "\". Switching to \"auto\"." << std::endl;
      }
      return "auto";
  }
}

std::string
dir_to_translated_string(const Direction& dir)
{
  switch (dir)
  {
    case Direction::NONE:
      return _("none");
    case Direction::LEFT:
      return _("left");
    case Direction::RIGHT:
      return _("right");
    case Direction::UP:
      return _("up");
    case Direction::DOWN:
      return _("down");
    default:
      if (dir != Direction::AUTO)
      {
        // Display a warning when an invalid direction has been provided.
        log_warning << "Unknown direction \"" << dir << "\". Switching to \"auto\"." << std::endl;
      }
      return _("auto");
  }
}

Direction
string_to_dir(const std::string& dir_str)
{
  if (dir_str == "none")
    return Direction::NONE;
  else if (dir_str == "left")
    return Direction::LEFT;
  else if (dir_str == "right")
    return Direction::RIGHT;
  else if (dir_str == "up")
    return Direction::UP;
  else if (dir_str == "down")
    return Direction::DOWN;
  else
  {
    if (dir_str != "auto")
    {
      // Display a warning when an invalid direction has been provided.
      log_warning << "Unknown direction \"" << dir_str << "\". Switching to \"auto\"." << std::endl;
    }
    return Direction::AUTO;
  }
}

Direction
invert_dir(const Direction& dir)
{
  switch (dir)
  {
    case Direction::RIGHT:
      return Direction::LEFT;
    case Direction::LEFT:
      return Direction::RIGHT;
    case Direction::DOWN:
      return Direction::UP;
    case Direction::UP:
      return Direction::DOWN;
    default:
      return Direction::NONE;
  }
}

/* EOF */
