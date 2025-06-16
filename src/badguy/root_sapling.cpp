//  SuperTux - Root Sapling Badguy
//  Copyright (C) 2023 MatusGuy
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

#include "badguy/root_sapling.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/root.hpp"
#include "collision/collision_system.hpp"
#include "math/random.hpp"
#include "object/player.hpp"
#include "object/tilemap.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

static const float ROOT_SAPLING_RANGE = 32.f * 20;
static const float ROOT_SAPLING_SPAWN_TIME = 1.35f;

RootSapling::RootSapling(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/mole/corrupted/root_sapling.sprite", Direction::UP,
         LAYER_TILES - 15, "images/creatures/mole/corrupted/core_glow/core_glow.sprite"),
  m_root_timer(),
  m_dead(false)
{
  m_physic.enable_gravity(false);
  set_colgroup_active(COLGROUP_MOVING);
  set_action("idle", m_dir);

  m_glowing = true;

  SoundManager::current()->preload("sounds/squish.wav");
  SoundManager::current()->preload("sounds/fall.wav");
}

void
RootSapling::kill_fall()
{
  m_dead = true;
  m_glowing = false;

  SoundManager::current()->play("sounds/fall.wav", get_pos());

  set_colgroup_active(COLGROUP_DISABLED);
  set_action("squished", m_dir);

  run_dead_script();
}

HitResponse
RootSapling::collision_badguy(BadGuy& other, const CollisionHit& hit)
{
  if (other.is_frozen())
  {
    kill_fall();
    return FORCE_MOVE;
  }

  return BadGuy::collision_badguy(other, hit);
}

bool
RootSapling::collision_squished(MovingObject& object)
{
  m_dead = true;
  m_glowing = false;

  SoundManager::current()->play("sounds/squish.wav", get_pos());

  set_action("squished", m_dir);
  set_colgroup_active(COLGROUP_DISABLED);

  auto player = dynamic_cast<Player*>(&object);
  if (player)
    player->bounce(*this);

  run_dead_script();
  return true;
}

HitResponse
RootSapling::collision_player(Player& player, const CollisionHit& hit)
{
  if (m_dir != Direction::DOWN || !hit.bottom)
    return BadGuy::collision_player(player, hit);

  player.get_physic().set_velocity_y(-player.get_physic().get_velocity_y());
  kill_fall();
  return FORCE_MOVE;
}

void
RootSapling::active_update(float dt_sec)
{
  if (m_dead) return;

  BadGuy::active_update(dt_sec);

  Player* player = get_nearest_player();
  if (!player) return;

  float distance = std::abs(get_pos().x - player->get_pos().x);
  if (distance > ROOT_SAPLING_RANGE) return;

  if (!m_root_timer.started() || m_root_timer.check())
  {
    summon_root();
    m_root_timer.start(ROOT_SAPLING_SPAWN_TIME);
  }
}

std::vector<Direction>
RootSapling::get_allowed_directions() const
{
  return { Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT };
}

void
RootSapling::summon_root()
{
  for (Player* player : Sector::get().get_players())
  {
    Vector pos;
    float* axis = nullptr;
    switch (m_dir)
    {
      case Direction::UP:
      case Direction::DOWN:
        pos.x = player->get_bbox().get_middle().x;
        axis = &pos.y;
        break;

      case Direction::LEFT:
      case Direction::RIGHT:
        pos.y = player->get_bbox().get_middle().y;
        axis = &pos.x;
        break;

      default: assert(false); break;
    }

    if (player->on_ground() && m_dir == Direction::UP)
    {
      (*axis) = player->get_bbox().get_bottom() + 1;

      bool should_summon = false;
      for (TileMap* tilemap : Sector::get().get_solid_tilemaps())
      {
        const Tile& tile = tilemap->get_tile_at(pos);
        if (tile.is_solid())
        {
          should_summon = true;
          break;
        }
      }
      if (!should_summon) continue;
    }
    else
    {
      using RaycastResult = CollisionSystem::RaycastResult;

      Vector eye, end;
      switch (m_dir)
      {
        case Direction::UP:
          eye = { player->get_bbox().get_middle().x, player->get_bbox().get_bottom() + 1 };
          end = { eye.x, eye.y + ROOT_SAPLING_RANGE };
          break;

        case Direction::DOWN:
          eye = { player->get_bbox().get_middle().x, player->get_bbox().get_top() - 1 };
          end = { eye.x, eye.y - ROOT_SAPLING_RANGE };
          break;

        case Direction::LEFT:
          eye = { player->get_bbox().get_right() + 1, player->get_bbox().get_middle().y };
          end = { eye.x + ROOT_SAPLING_RANGE, eye.y };
          break;

        case Direction::RIGHT:
          eye = { player->get_bbox().get_left() - 1, player->get_bbox().get_middle().y };
          end = { eye.x - ROOT_SAPLING_RANGE, eye.y };
          break;

        default: assert(false); break;
      }

      RaycastResult result = m_dir == Direction::LEFT || m_dir == Direction::UP ?
                             Sector::get().get_first_line_intersection(eye, end, true, nullptr) :
                             reverse_raycast(eye, end);

      auto tile_p = std::get_if<const Tile*>(&result.hit);
      if (!tile_p || result.box.empty())
        continue;

      switch (m_dir)
      {
        case Direction::UP:
        case Direction::DOWN:
          if ((*tile_p)->is_unisolid())
            continue;
          (*axis) = result.box.p1().y;
          break;

        case Direction::LEFT:
        case Direction::RIGHT:
          if ((*tile_p)->is_unisolid())
            continue;
          (*axis) = result.box.p1().x;
          break;

        default: assert(false); break;
      }

      Sizef size(32.f, 32.f);
      switch (m_dir)
      {
        case Direction::UP: size.height *= 3; break;
        case Direction::DOWN: size.height *= 3; break;
        case Direction::LEFT: size.width *= 3; break;
        case Direction::RIGHT: size.width *= 3; break;
        default: assert(false); break;
      }

      Vector bboxpos = pos;
      switch (m_dir)
      {
        case Direction::DOWN:
          bboxpos.y = std::max(bboxpos.y - (32.f * 3.f), 0.f);
          [[fallthrough]];
        case Direction::UP:
          bboxpos.x -= 16.f;
          break;

        case Direction::RIGHT:
          bboxpos.x = std::max(bboxpos.x - (32.f * 3.f), 0.f);
          [[fallthrough]];
        case Direction::LEFT:
          bboxpos.y -= 16.f;
          break;

        default: assert(false); break;
      }

      // Check if the hitbox of the root is entirely
      // occupied by solid tiles.
      Rectf space(bboxpos, size);
      if (!should_summon_root(space.grown(-1)))
        continue;

      Sector::get().add<Root>(pos, m_dir, "images/creatures/mole/corrupted/root.sprite");
    }
  }
}

bool
RootSapling::should_summon_root(const Rectf& bbox)
{
  for (const auto& solids : Sector::get().get_solid_tilemaps())
  {
    if (solids->get_path())
      continue; // Do not support moving tilemaps. Not planned.

    // Test with all tiles in the root's hitbox
    const Rect test_tiles = solids->get_tiles_overlapping(bbox);

    for (int x = test_tiles.left; x < test_tiles.right; ++x)
    {
      for (int y = test_tiles.top; y < test_tiles.bottom; ++y)
      {
        const Tile& tile = solids->get_tile(x, y);

        if (!(tile.get_attributes() & Tile::SOLID))
          goto next_tilemap;
      }
    }

    return true;

next_tilemap:
    ;
  }

  return false;
}

CollisionSystem::RaycastResult
RootSapling::reverse_raycast(const Vector& line_start, const Vector& line_end)
{
  /*
   * This "Raycasting" I've been using so far is not actual raycasting.
   * It just checks for tiles/objects that intersect a line and returns
   * the closest one to the top left (or whatever order it's on). Which
   * means it only works properly when pointing down or right. If you
   * try otherwise it will return the furthest tile/object in the line.
   *
   * FIXME: Rename this to "Intersection checking" or
   * make it support other directions.
   *
   * Anyway, to work around this, copy the current intersection checking
   * function and alter it to go in reverse
   *
   * Usually, when I write big comments, that means I won't need it. I hope
   * that's the case.
   *
   * ~ MatusGuy
   */

  CollisionSystem::RaycastResult result{};

  // Check if no tile is in the way.
  const float lsx = std::max(line_start.x, line_end.x);
  const float lex = std::min(line_start.x, line_end.x);
  const float lsy = std::max(line_start.y, line_end.y);
  const float ley = std::min(line_start.y, line_end.y);

  for (float test_x = lsx; test_x >= lex; test_x -= 16) { // NOLINT.
    for (float test_y = lsy; test_y >= ley; test_y -= 16) { // NOLINT.
      for (const auto& solids : Sector::get().get_solid_tilemaps()) {
        Vector test_vector(test_x, test_y);
        if (solids->is_outside_bounds(test_vector))
          continue;

        const Tile* tile = &solids->get_tile_at(test_vector);

        if (tile->get_attributes() & Tile::SOLID)
        {
          result.is_valid = true;
          result.hit = tile;
          result.box = solids->get_tile_bbox(static_cast<int>(test_vector.x / 32.f)+1, static_cast<int>(test_vector.y / 32.f)+1);
          return result;
        }
      }
    }
  }

  result.is_valid = false;
  return result;
}

void
RootSapling::on_flip(float height)
{
  BadGuy::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
  m_dir = invert_dir(m_dir);
}

/* EOF */
