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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SDL_TEXTURE_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SDL_TEXTURE_HPP

#include "video/texture.hpp"

#include <SDL.h>

#include "video/sampler.hpp"

class SDLTexture final : public Texture
{
public:
  SDLTexture(SDL_Texture* texture, int width, int height, const Sampler& sampler);
  SDLTexture(const SDL_Surface& image, const Sampler& sampler);
  ~SDLTexture() override;

  virtual void reload(const SDL_Surface& image) override;

  virtual int get_texture_width() const override { return m_width; }
  virtual int get_texture_height() const override { return m_height; }

  virtual int get_image_width() const override { return m_width; }
  virtual int get_image_height() const override { return m_height; }

  inline SDL_Texture *get_texture() const { return m_texture; }
  inline const Sampler& get_sampler() const { return m_sampler; }

private:
  SDL_Texture* m_texture;
  int m_width;
  int m_height;

private:
  SDLTexture(const SDLTexture&) = delete;
  SDLTexture& operator=(const SDLTexture&) = delete;
};

#endif

/* EOF */
