//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023-2024 Vankata453
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

#include "sprite/sprite_manager.hpp"

#include "sprite/sprite.hpp"

SpriteManager::SpriteManager() :
  m_sprites()
{
}

SpritePtr
SpriteManager::create(const std::string& name)
{
  Sprites::iterator i = m_sprites.find(name);
  SpriteData* data;
  if (i == m_sprites.end())
  {
    // Try loading the sprite file.
    data = load(name);
  }
  else
  {
    data = i->second.get();
  }

  return SpritePtr(new Sprite(*data));
}

SpriteData*
SpriteManager::load(const std::string& filename)
{
  m_sprites[filename] = std::make_unique<SpriteData>(filename);
  return m_sprites[filename].get();
}

void
SpriteManager::reload()
{
  for (const auto& sprite_data : m_sprites)
    sprite_data.second->load();
}

/* EOF */
