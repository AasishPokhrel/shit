//  SuperTux
//  Copyright (C) 2009 Mathnerd314
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

#ifndef HEADER_SUPERTUX_SUPERTUX_CONSTANTS_HPP
#define HEADER_SUPERTUX_SUPERTUX_CONSTANTS_HPP

#include <string>

// the engine will be run with a logical framerate of 66.666fps, corresponding
// to a 15 msec gap between steps. Warning: changing this may affect physics
static const float LOGICAL_FPS = 1000.0f / 15.0f;

// SHIFT_DELTA is used for sliding over 1-tile gaps and collision detection
static const float SHIFT_DELTA = 7.0f;

// a small value... be careful as collision detection is very sensitive to it
static const float EPSILON = .002f;

// The spawnpoint that gets activated at the start of a game session
static const std::string DEFAULT_SPAWNPOINT_NAME = "main";

// The sector that gets activated by default when a level is started
static const std::string DEFAULT_SECTOR_NAME = "main";

#endif

/* EOF */
