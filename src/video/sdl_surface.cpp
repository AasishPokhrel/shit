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

#include "video/sdl_surface.hpp"

#include <sstream>
#include <stdexcept>

#include <SDL_image.h>
#include <savepng.h>

#include "physfs/physfs_sdl.hpp"
#include "util/log.hpp"

SDLSurfacePtr
SDLSurface::create_rgba(int width, int height)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 rmask = 0xff000000;
  Uint32 gmask = 0x00ff0000;
  Uint32 bmask = 0x0000ff00;
  Uint32 amask = 0x000000ff;
#else
  Uint32 rmask = 0x000000ff;
  Uint32 gmask = 0x0000ff00;
  Uint32 bmask = 0x00ff0000;
  Uint32 amask = 0xff000000;
#endif
  SDLSurfacePtr surface(SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask));
  if (!surface) {
    std::ostringstream out;
    out << "failed to create SDL_Surface: " << SDL_GetError();
    throw std::runtime_error(out.str());
  }

  return surface;
}

SDLSurfacePtr
SDLSurface::create_rgb(int width, int height)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 rmask = 0xff000000;
  Uint32 gmask = 0x00ff0000;
  Uint32 bmask = 0x0000ff00;
  Uint32 amask = 0x00000000;
#else
  Uint32 rmask = 0x000000ff;
  Uint32 gmask = 0x0000ff00;
  Uint32 bmask = 0x00ff0000;
  Uint32 amask = 0x00000000;
#endif
  SDLSurfacePtr surface(SDL_CreateRGBSurface(0, width, height, 24, rmask, gmask, bmask, amask));
  if (!surface) {
   std::ostringstream out;
    out << "failed to create SDL_Surface: " << SDL_GetError();
    throw std::runtime_error(out.str());
  }

  return surface;
}

SDLSurfacePtr
SDLSurface::from_file(const std::string& filename)
{
  log_debug << "loading image: " << filename << std::endl;
  SDLSurfacePtr surface(IMG_Load_RW(get_physfs_SDLRWops(filename), 1));
  if (!surface)
  {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    return surface;
  }
}

int
SDLSurface::save_png(const SDL_Surface& surface, const std::string& filename)
{
  // This does not lead to a double free when 'tmp == screen', as
  // SDL_PNGFormatAlpha() will increase the refcount of surface.
  SDLSurfacePtr tmp(SDL_PNGFormatAlpha(const_cast<SDL_Surface*>(&surface)));
  SDL_RWops* ops;
  try {
    ops = get_writable_physfs_SDLRWops(filename);
  } catch (std::exception& e) {
    log_warning << "Could not get SDLRWops for " << filename << ": " <<
      e.what() << std::endl;
    return false;
  }
  int ret = SDL_SavePNG_RW(tmp.get(), ops, 1);
  if (ret < 0)
  {
    log_warning << "Saving " << filename << " failed: " << SDL_GetError() << std::endl;
    return false;
  }
  else
  {
    return true;
  }
}

/* EOF */
