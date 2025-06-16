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

#include "object/particlesystem_interactive.hpp"

#include "collision/collision.hpp"
#include "math/aatriangle.hpp"
#include "object/tilemap.hpp"
#include "supertux/globals.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"
#include "video/drawing_context.hpp"
#include "video/surface_batch.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

//TODO: Find a way to make rain collide with objects like bonus blocks
//      Add an option to set rain strength
//      Fix rain being "respawned" over solid tiles
ParticleSystem_Interactive::ParticleSystem_Interactive() :
  ParticleSystem()
{
  virtual_width = static_cast<float>(SCREEN_WIDTH);
  virtual_height = static_cast<float>(SCREEN_HEIGHT);
}

ParticleSystem_Interactive::ParticleSystem_Interactive(const ReaderMapping& mapping) :
  ParticleSystem(mapping)
{
  virtual_width = static_cast<float>(SCREEN_WIDTH);
  virtual_height = static_cast<float>(SCREEN_HEIGHT);
}

ParticleSystem_Interactive::~ParticleSystem_Interactive()
{
}

void
ParticleSystem_Interactive::draw(DrawingContext& context)
{
  if (!enabled)
    return;

  context.push_transform();
  const auto& region = Sector::current()->get_active_region();
  std::unordered_map<SurfacePtr, SurfaceBatch> batches;
  for (const auto& particle : particles) {
    if(!region.contains(particle->pos))
      continue;

    auto it = batches.find(particle->texture);
    if (it == batches.end()) {
      const auto& batch_it = batches.emplace(particle->texture,
        SurfaceBatch(particle->texture));
      batch_it.first->second.draw(particle->pos, particle->angle);
    } else {
      it->second.draw(particle->pos, particle->angle);
    }
  }

  for(auto& it : batches) {
    auto& surface = it.first;
    auto& batch = it.second;
    // FIXME: What is the colour used for?
    context.color().draw_surface_batch(surface, batch.move_srcrects(),
      batch.move_dstrects(), batch.move_angles(), Color::WHITE, z_pos);
  }

  context.pop_transform();
}

int
ParticleSystem_Interactive::collision(Particle* object, const Vector& movement)
{
  using namespace collision;

  // calculate rectangle where the object will move
  float x1, x2;
  float y1, y2;

  x1 = object->pos.x;
  x2 = x1 + 32 + movement.x;
  if (x2 < x1) {
    x1 = x2;
    x2 = object->pos.x;
  }

  y1 = object->pos.y;
  y2 = y1 + 32 + movement.y;
  if (y2 < y1) {
    y1 = y2;
    y2 = object->pos.y;
  }
  bool water = false;

  // test with all tiles in this rectangle
  int starttilex = int(x1-1) / 32;
  int starttiley = int(y1-1) / 32;
  int max_x = int(x2+1);
  int max_y = int(y2+1);

  Rectf dest(x1, y1, x2, y2);
  dest.move(movement);
  Constraints constraints;

  for (const auto& solids : Sector::get().get_solid_tilemaps()) {
    // FIXME Handle a nonzero tilemap offset
    for (int x = starttilex; x*32 < max_x; ++x) {
      for (int y = starttiley; y*32 < max_y; ++y) {
        const Tile& tile = solids->get_tile(x, y);

        // skip non-solid tiles, except water
        if (! (tile.get_attributes() & (Tile::WATER | Tile::SOLID)))
          continue;

        Rectf rect = solids->get_tile_bbox(x, y);
        if (tile.is_slope ()) { // slope tile
          AATriangle triangle = AATriangle(rect, tile.get_data());

          if (rectangle_aatriangle(&constraints, dest, triangle)) {
            if (tile.get_attributes() & Tile::WATER)
              water = true;
          }
        } else { // normal rectangular tile
          if (dest.overlaps(rect)) {
            if (tile.get_attributes() & Tile::WATER)
              water = true;
            set_rectangle_rectangle_constraints(&constraints, dest, rect);
          }
        }
      }
    }
  }

  // TODO don't use magic numbers here...

  // did we collide at all?
  if (!constraints.has_constraints())
    return -1;

  const CollisionHit& hit = constraints.hit;
  if (water) {
    return 0; //collision with water tile - don't draw splash
  } else {
    if (hit.right || hit.left) {
      return 2; //collision from right
    } else {
      return 1; //collision from above
    }
  }
}

/* EOF */
