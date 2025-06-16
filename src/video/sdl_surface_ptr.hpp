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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SURFACE_PTR_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SURFACE_PTR_HPP

#include <SDL.h>

/** Simple Wrapper class around SDL_Surface that provides exception
    safety */
class SDLSurfacePtr final
{
private:
  SDL_Surface* m_surface;

public:
  SDLSurfacePtr() :
    m_surface(nullptr)
  {}

  explicit SDLSurfacePtr(SDL_Surface* surface) :
    m_surface(surface)
  {}

  SDLSurfacePtr(SDLSurfacePtr&& other) noexcept :
    m_surface(other.m_surface)
  {
    other.m_surface = nullptr;
  }

  SDLSurfacePtr& operator=(SDLSurfacePtr&& other) noexcept
  {
    if (this != &other)
    {
      m_surface = other.m_surface;
      other.m_surface = nullptr;
    }
    return *this;
  }

  ~SDLSurfacePtr()
  {
    SDL_FreeSurface(m_surface);
  }

  SDL_Surface& operator*()
  {
    return *m_surface;
  }

  const SDL_Surface& operator*() const
  {
    return *m_surface;
  }

  SDL_Surface* operator->()
  {
    return m_surface;
  }

  const SDL_Surface* operator->() const
  {
    return m_surface;
  }

  void reset(SDL_Surface* surface)
  {
    SDL_FreeSurface(m_surface);
    m_surface = surface;
  }

  void reset(SDLSurfacePtr& other)
  {
    SDL_FreeSurface(m_surface);
    m_surface = other.m_surface;
    other.m_surface = nullptr;
  }

  SDL_Surface* get() const
  {
    return m_surface;
  }

  operator void*() {
    return m_surface;
  }

private:
  SDLSurfacePtr(const SDLSurfacePtr&) = delete;
  SDLSurfacePtr& operator=(const SDLSurfacePtr&) = delete;
};

#endif

/* EOF */
