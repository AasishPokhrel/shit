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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_TEXTURE_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_TEXTURE_HPP

#include <optional>

#include "video/color.hpp"
#include "video/gl.hpp"
#include "video/sampler.hpp"
#include "video/texture.hpp"

/** This class is a wrapper around a texture handle. It stores the
    texture width and height and provides convenience functions for
    uploading SDL_Surfaces into the texture. */
class GLTexture final : public Texture
{
public:
  GLTexture(int width, int height, std::optional<Color> fill_color = std::nullopt);
  GLTexture(const SDL_Surface& image, const Sampler& sampler);
  ~GLTexture() override;

  virtual void reload(const SDL_Surface& image) override;

  virtual int get_texture_width() const override { return m_texture_width; }
  virtual int get_texture_height() const override { return m_texture_height; }

  virtual int get_image_width() const override { return m_image_width; }
  virtual int get_image_height() const override { return m_image_height; }

  inline void set_handle(GLuint handle) { m_handle = handle; }
  inline const GLuint &get_handle() const { return m_handle; }

  inline const Sampler& get_sampler() const { return m_sampler; }

  inline void set_image_width(int width) { m_image_width = width; }
  inline void set_image_height(int height) { m_image_height = height; }

private:
  void set_texture_params();

private:
  GLuint m_handle;
  int m_texture_width;
  int m_texture_height;
  int m_image_width;
  int m_image_height;

private:
  GLTexture(const GLTexture&) = delete;
  GLTexture& operator=(const GLTexture&) = delete;
};

#endif

/* EOF */
