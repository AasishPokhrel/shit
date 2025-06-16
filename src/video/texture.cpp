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

#include "video/texture.hpp"

#include "video/texture_manager.hpp"

Texture::Texture() :
  m_sampler(),
  m_cache_key()
{
}

Texture::Texture(const Sampler& sampler) :
  m_sampler(sampler),
  m_cache_key()
{
}

Texture::~Texture()
{
  if (TextureManager::current() && m_cache_key)
  {
    // The cache entry is now useless: its weak pointer to us has
    // been cleared. Remove the entry altogether to save memory.
    TextureManager::current()->reap_cache_entry(*m_cache_key);
  }
}

/* EOF */
