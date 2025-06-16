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

#include "video/null/null_texture.hpp"

NullTexture::NullTexture(const Size& size) :
  m_texture_size(size),
  m_image_size(size)
{
}

NullTexture::~NullTexture()
{
}

void
NullTexture::reload(const SDL_Surface&)
{
}

int
NullTexture::get_texture_width() const
{
  return m_texture_size.width;
}

int
NullTexture::get_texture_height() const
{
  return m_texture_size.height;
}

int
NullTexture::get_image_width() const
{
  return m_image_size.width;
}

int
NullTexture::get_image_height() const
{
  return m_image_size.height;
}

/* EOF */
