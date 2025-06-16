//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_TTF_SURFACE_HPP
#define HEADER_SUPERTUX_VIDEO_TTF_SURFACE_HPP

#include <memory>
#include <string>

#include "math/vector.hpp"
#include "video/surface_ptr.hpp"

class TTFFont;
class TTFSurface;

typedef std::shared_ptr<TTFSurface> TTFSurfacePtr;

/** TTFSurface class holds a rendered string */
class TTFSurface final
{
public:
  static TTFSurfacePtr create(const TTFFont& font, const std::string& text);

public:
  TTFSurface(const SurfacePtr& surface, const Vector& offset);

  inline SurfacePtr get_surface() { return m_surface; }
  inline Vector get_offset() const { return m_offset; }

  int get_width() const;
  int get_height() const;

private:
  SurfacePtr m_surface;
  Vector m_offset;

private:
  TTFSurface(const TTFSurface&) = delete;
  TTFSurface& operator=(const TTFSurface&) = delete;
};

#endif

/* EOF */
