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

#ifndef HEADER_SUPERTUX_SUPERTUX_GLOBALS_HPP
#define HEADER_SUPERTUX_SUPERTUX_GLOBALS_HPP

#include <memory>

class Config;

// This is meant to be temporarily. Code should not use
// SCREEN_WIDTH/HEIGHT, but instead use context.get_width()/height()
// inside the draw() call.
#define SCREEN_WIDTH (VideoSystem::current()->get_viewport().get_screen_width())
#define SCREEN_HEIGHT (VideoSystem::current()->get_viewport().get_screen_height())

extern Config* g_config;

extern float g_game_time;
extern float g_real_time;

#endif

/* EOF */
