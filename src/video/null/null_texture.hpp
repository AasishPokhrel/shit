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

#ifndef HEADER_SUPERTUX_VIDEO_NULL_NULL_TEXTURE_HPP
#define HEADER_SUPERTUX_VIDEO_NULL_NULL_TEXTURE_HPP

#include "video/texture.hpp"

#include "math/size.hpp"

class NullTexture : public Texture
{
public:
  NullTexture(const Size& size);
  ~NullTexture() override;

  virtual void reload(const SDL_Surface& image) override;

  virtual int get_texture_width() const override;
  virtual int get_texture_height() const override;

  virtual int get_image_width() const override;
  virtual int get_image_height() const override;

private:
  Size m_texture_size;
  Size m_image_size;

private:
  NullTexture(const NullTexture&) = delete;
  NullTexture& operator=(const NullTexture&) = delete;
};

#endif

/* EOF */
