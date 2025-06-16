//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SURFACE_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SURFACE_HPP

#include <string>
#include <SDL.h>

#include "video/sdl_surface_ptr.hpp"

class SDLSurface final
{
public:
  static SDLSurfacePtr create_rgba(int width, int height);
  static SDLSurfacePtr create_rgb(int width, int height);
  static SDLSurfacePtr from_file(const std::string& filename);
  static int save_png(const SDL_Surface& surface, const std::string& filename);
};

#endif

/* EOF */
