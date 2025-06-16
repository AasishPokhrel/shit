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

#include "video/gl/gl_texture.hpp"

#include <assert.h>

#include "video/glutil.hpp"
#include "video/sampler.hpp"
#include "video/sdl_surface.hpp"

GLTexture::GLTexture(int width, int height, std::optional<Color> fill_color) :
  m_handle(),
  m_texture_width(),
  m_texture_height(),
  m_image_width(),
  m_image_height()
{
#ifdef GL_VERSION_ES_CM_1_0
  assert(is_power_of_2(width));
  assert(is_power_of_2(height));
#endif
  m_texture_width  = width;
  m_texture_height = height;
  m_image_width  = width;
  m_image_height = height;

  assert_gl();

  glGenTextures(1, &m_handle);

  try {
    glBindTexture(GL_TEXTURE_2D, m_handle);

    if (fill_color)
    {
      std::vector<uint32_t> pixels(m_texture_width * m_texture_height, fill_color->rgba());
      glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA), m_texture_width,
                   m_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }
    else
    {
      glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA), m_texture_width,
                   m_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    set_texture_params();
  } catch(...) {
    glDeleteTextures(1, &m_handle);
    throw;
  }

  assert_gl();
}

GLTexture::GLTexture(const SDL_Surface& image, const Sampler& sampler) :
  Texture(sampler),
  m_handle(),
  m_texture_width(),
  m_texture_height(),
  m_image_width(),
  m_image_height()
{
  reload(image);
}

void
GLTexture::reload(const SDL_Surface& image)
{
  assert_gl();

  glDeleteTextures(1, &m_handle);

  if (gl_needs_power_of_two())
  {
    m_texture_width = next_power_of_two(image.w);
    m_texture_height = next_power_of_two(image.h);
  }
  else
  {
    m_texture_width  = image.w;
    m_texture_height = image.h;
  }

  m_image_width  = image.w;
  m_image_height = image.h;

  SDLSurfacePtr convert = SDLSurface::create_rgba(m_texture_width, m_texture_height);

  SDL_SetSurfaceBlendMode(const_cast<SDL_Surface*>(&image), SDL_BLENDMODE_NONE);
  SDL_BlitSurface(const_cast<SDL_Surface*>(&image), nullptr, convert.get(), nullptr);

  // Fill the remaining pixels of 'convert' with repeated copies of
  // 'image' to minimize OpenGL blending artifacts at the borders.
  if (m_image_width != m_texture_width || m_image_height != m_texture_height)
  {
    if (SDL_MUSTLOCK(convert)) {
      SDL_LockSurface(convert.get());
    }

    if (m_image_width != m_texture_width) {
      SDL_Rect srcrect{m_image_width - 1, 0, 1, m_image_height};
      for (int x = m_image_width; x < m_texture_width; ++x) {
        SDL_Rect dstrect{x, 0, 1, m_image_height};
        SDL_BlitSurface(const_cast<SDL_Surface*>(&image), &srcrect, convert.get(), &dstrect);
      }
    }

    if (m_image_height != m_texture_height) {
      SDL_Rect srcrect{0, m_image_height - 1, m_image_width, 1};
      for (int y = m_image_height; y < m_texture_height; ++y) {
        SDL_Rect dstrect{0, y, m_image_width, 1};
        SDL_BlitSurface(const_cast<SDL_Surface*>(&image), &srcrect, convert.get(), &dstrect);
      }
    }

    if (m_image_width != m_texture_width && m_image_height != m_texture_height)
    {
      const int bpp = convert->format->BytesPerPixel;
      const int x = m_image_width - 1;
      const int y = m_image_height - 1;
      Uint32 color = 0;
      memcpy(&color, static_cast<uint8_t*>(convert->pixels) + y * convert->pitch + x * bpp, bpp);
      SDL_Rect dstrect{m_image_width, m_image_height, m_texture_width, m_texture_height};
      SDL_FillRect(convert.get(), &dstrect, color);
    }

    if (SDL_MUSTLOCK(convert)) {
      SDL_UnlockSurface(convert.get());
    }
  }

  assert_gl();

  glGenTextures(1, &m_handle);

  try {
    GLenum sdl_format;
    if (convert->format->BytesPerPixel == 3) {
      sdl_format = GL_RGB;
    } else if (convert->format->BytesPerPixel == 4) {
      sdl_format = GL_RGBA;
    } else {
      sdl_format = GL_RGBA; // NOLINT.
      assert(false);
    }

    glBindTexture(GL_TEXTURE_2D, m_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#if defined(GL_UNPACK_ROW_LENGTH) || defined(USE_GLBINDING)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, convert->pitch/convert->format->BytesPerPixel);
#else
    /* OpenGL ES doesn't support UNPACK_ROW_LENGTH, let's hope SDL didn't add
     * padding bytes, otherwise we need some extra code here... */
    assert(convert->pitch == static_cast<int>(m_texture_width * convert->format->BytesPerPixel));
#endif

    if (SDL_MUSTLOCK(convert)) {
      SDL_LockSurface(convert.get());
    }

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA),
                 m_texture_width, m_texture_height, 0, sdl_format,
                 GL_UNSIGNED_BYTE, convert->pixels);

    // Disable the use of mipmaps for the texture.
#if 0
    glGenerateMipmap(GL_TEXTURE_2D);
#endif

    if (SDL_MUSTLOCK(convert.get())) {
      SDL_UnlockSurface(convert.get());
    }

    assert_gl();

    set_texture_params();
  } catch(...) {
    glDeleteTextures(1, &m_handle);
    throw;
  }

  assert_gl();
}

GLTexture::~GLTexture()
{
  glDeleteTextures(1, &m_handle);
}

void
GLTexture::set_texture_params()
{
  assert_gl();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(m_sampler.get_filter()));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(m_sampler.get_filter()));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(m_sampler.get_wrap_s()));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(m_sampler.get_wrap_t()));

  assert_gl();
}

/* EOF */
