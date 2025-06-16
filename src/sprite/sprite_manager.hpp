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

#ifndef HEADER_SUPERTUX_SPRITE_SPRITE_MANAGER_HPP
#define HEADER_SUPERTUX_SPRITE_SPRITE_MANAGER_HPP

#include "util/currenton.hpp"

#include <unordered_map>
#include <memory>
#include <string>

#include "sprite/sprite_ptr.hpp"

class SpriteData;

class SpriteManager final : public Currenton<SpriteManager>
{
private:
  typedef std::unordered_map<std::string, std::unique_ptr<SpriteData>> Sprites;
  Sprites m_sprites;

public:
  SpriteManager();

  /** Loads a sprite. */
  SpritePtr create(const std::string& filename);

  /** Reloads all sprites. */
  void reload();

private:
  SpriteData* load(const std::string& filename);

private:
  SpriteManager(const SpriteManager&) = delete;
  SpriteManager& operator=(const SpriteManager&) = delete;
};

#endif

/* EOF */
