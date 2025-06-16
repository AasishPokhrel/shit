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

#include "supertux/flip_level_transformer.hpp"

#include "badguy/badguy.hpp"
#include "object/block.hpp"
#include "object/camera.hpp"
#include "object/decal.hpp"
#include "object/flower.hpp"
#include "object/platform.hpp"
#include "object/player.hpp"
#include "object/tilemap.hpp"
#include "supertux/sector.hpp"

void
FlipLevelTransformer::transform_sector(Sector& sector)
{
  float height = sector.get_height();

  for (auto& object : sector.get_objects())
  {
    if (!object->is_valid())
      return;

    object->on_flip(height);
  }

  sector.get_camera().reset(sector.get_players()[0]->get_pos());
}

void
FlipLevelTransformer::transform_flip(Flip& flip)
{
  if (flip & VERTICAL_FLIP) {
    flip = flip & ~VERTICAL_FLIP;
  } else {
    flip = flip | VERTICAL_FLIP;
  }
}

/* EOF */
