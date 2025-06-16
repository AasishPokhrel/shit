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

#ifndef HEADER_SUPERTUX_VIDEO_TEXTURE_HPP
#define HEADER_SUPERTUX_VIDEO_TEXTURE_HPP

#include <string>
#include <tuple>
#include <optional>

#include "math/rect.hpp"
#include "video/flip.hpp"
#include "video/sampler.hpp"

struct SDL_Surface;

/** This class is a wrapper around a texture handle. It stores the
    texture width and height and provides convenience functions for
    uploading SDL_Surfaces into the texture. */
class Texture
{
  friend class TextureManager;

public:
  /** filename, left, top, right, bottom */
  using Key = std::tuple<std::string, Rect>;

protected:
  Texture();
  Texture(const Sampler& sampler);

public:
  virtual ~Texture();

  virtual void reload(const SDL_Surface& image) = 0;

  virtual int get_texture_width() const = 0;
  virtual int get_texture_height() const = 0;

  virtual int get_image_width() const = 0;
  virtual int get_image_height() const = 0;

  inline const Sampler& get_sampler() const { return m_sampler; }

protected:
  Sampler m_sampler;

private:
  std::optional<Key> m_cache_key;

private:
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
};

#endif

/* EOF */
