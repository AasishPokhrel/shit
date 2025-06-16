//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_DIRECTION_HPP
#define HEADER_SUPERTUX_SUPERTUX_DIRECTION_HPP

#include <iostream>

template<typename T> class ObjectOption;

enum class Direction { AUTO, NONE, LEFT, RIGHT, UP, DOWN };

std::ostream& operator<<(std::ostream& o, const Direction& dir);

std::string dir_to_string(const Direction& dir);
std::string dir_to_translated_string(const Direction& dir);
Direction string_to_dir(const std::string& dir_str);
Direction invert_dir(const Direction& dir);

#endif

/* EOF */
