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

#ifndef HEADER_SUPERTUX_VIDEO_SURFACE_HPP
#define HEADER_SUPERTUX_VIDEO_SURFACE_HPP

#include <string>
#include <optional>

#include "math/rect.hpp"
#include "math/vector.hpp"
#include "video/flip.hpp"
#include "video/surface_ptr.hpp"
#include "video/texture_ptr.hpp"

class ReaderMapping;
class SurfaceData;

/** A rectangular image.  The class basically holds a reference to a
    texture with additional UV coordinates that specify a rectangular
    area on this texture */
class Surface final
{
public:
  static SurfacePtr from_texture(const TexturePtr& texture);
  static SurfacePtr from_file(const std::string& filename, const std::optional<Rect>& rect = std::nullopt);
  static SurfacePtr from_reader(const ReaderMapping& mapping, const std::optional<Rect>& rect = std::nullopt, const std::string& filename = "");

private:
  Surface(const TexturePtr& diffuse_texture, const TexturePtr& displacement_texture, Flip flip, const std::string& filename = "");
  Surface(const TexturePtr& diffuse_texture, const TexturePtr& displacement_texture, const Rect& region, Flip flip, const std::string& filename = "");

public:
  ~Surface();

  SurfacePtr region(const Rect& rect) const;
  SurfacePtr clone(Flip flip = NO_FLIP) const;

  TexturePtr get_texture() const;
  TexturePtr get_displacement_texture() const;
  inline Rect get_region() const { return m_region; }
  int get_width() const;
  int get_height() const;
  inline Flip get_flip() const { return m_flip; }
  inline const std::string& get_filename() const { return m_source_filename; }

private:
  const TexturePtr m_diffuse_texture;
  const TexturePtr m_displacement_texture;
  const Rect m_region;
  const Flip m_flip;
  const std::string m_source_filename;

private:
  Surface& operator=(const Surface&) = delete;
};

#endif

/* EOF */
