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

#ifndef HEADER_SUPERTUX_SUPERTUX_FLIP_LEVEL_TRANSFORMER_HPP
#define HEADER_SUPERTUX_SUPERTUX_FLIP_LEVEL_TRANSFORMER_HPP

#include "supertux/level_transformer.hpp"
#include "video/drawing_context.hpp"

class TileMap;
class BadGuy;
class SpawnPoint;
class MovingObject;
class Flower;
class Platform;
class Block;
class Path;
class Decal;

/** Vertically or horizontally flip a level */
class FlipLevelTransformer final : public LevelTransformer
{
public:
  static void transform_flip(Flip& flip);
  static void transform_path(float height, float obj_height, Path& path);
public:
  virtual void transform_sector(Sector& sector) override;
};

#endif

/* EOF */
