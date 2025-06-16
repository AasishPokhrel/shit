//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_LAYER_HPP
#define HEADER_SUPERTUX_VIDEO_LAYER_HPP

// some constants for predefined layer values
enum {
  // Image/gradient backgrounds (should cover entire screen)
  LAYER_BACKGROUND0 = -300,
  // Particle backgrounds
  LAYER_BACKGROUND1 = -200,
  // Tilemap backgrounds
  LAYER_BACKGROUNDTILES = -100,
  // Solid tilemaps
  LAYER_TILES = 0,
  // Ordinary objects
  LAYER_OBJECTS = 50,
  // Objects that pass through walls
  LAYER_FLOATINGOBJECTS = 150,
  //
  LAYER_FOREGROUNDTILES = 200,
  //
  LAYER_FOREGROUND0 = 300,
  //
  LAYER_FOREGROUND1 = 400,

  LAYER_LIGHTMAP = 450,

  // Used for the fog created by particles
  LAYER_FOG = 199,

  // Hitpoints, time, coins, etc.
  LAYER_HUD = 500,
  // Menus, mouse, console etc.
  LAYER_GUI = 600,

  // Make sure all get_light requests are handled last.
  LAYER_GETPIXEL = LAYER_GUI
};

#endif

/* EOF */
