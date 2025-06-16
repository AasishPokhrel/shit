//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "collision/collision_system.hpp"

#include "collision/collision.hpp"
#include "collision/collision_movement_manager.hpp"
#include "editor/editor.hpp"
#include "math/aatriangle.hpp"
#include "math/rect.hpp"
#include "object/player.hpp"
#include "object/tilemap.hpp"
#include "supertux/constants.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

namespace
{
  const float MAX_SPEED = 16.0f;
} // namespace

CollisionSystem::CollisionSystem(Sector& sector) :
  m_sector(sector),
  m_objects(),
  m_ground_movement_manager(new CollisionGroundMovementManager)
{
}

void
CollisionSystem::add(CollisionObject* object)
{
  object->set_ground_movement_manager(m_ground_movement_manager);
  m_objects.push_back(object);
}

void
CollisionSystem::remove(CollisionObject* object)
{
  m_objects.erase(
    std::find(m_objects.begin(), m_objects.end(),
      object));

  // FIXME: This is a patch. A better way of fixing this is coming.
  for (auto* collision_object : m_objects) {
    collision_object->notify_object_removal(object);
  }
  for (auto* tilemap : m_sector.get_solid_tilemaps()) {
    tilemap->notify_object_removal(object);
  }
}

void
CollisionSystem::draw(DrawingContext& context)
{
  const Color violet(0.5f, 0.0f, 1.0f, 0.75f);
  const Color red(1.0f, 0.0f, 0.0f, 0.75f);
  const Color red_bright(1.0f, 0.5f, 0.5f, 0.75f);
  const Color cyan(0.0f, 1.0f, 1.0f, 0.75f);
  const Color orange(1.0f, 0.5f, 0.0f, 0.75f);
  const Color green_bright(0.7f, 1.0f, 0.7f, 0.75f);
  for (auto& object : m_objects) {
    Color color;
    switch (object->get_group()) {
    case COLGROUP_MOVING_STATIC:
      color = violet;
      break;
    case COLGROUP_MOVING:
      color = red;
      break;
    case COLGROUP_MOVING_ONLY_STATIC:
      color = red_bright;
      break;
    case COLGROUP_STATIC:
      color = cyan;
      break;
    case COLGROUP_TOUCHABLE:
      color = orange;
      break;
    default:
      color = green_bright;
    }
    const Rectf& rect = object->get_bbox();
    context.color().draw_filled_rect(rect, color, LAYER_FOREGROUND1 + 10);

    // If unisolid, draw a line on top of the rectangle.
    if (object->is_unisolid())
      context.color().draw_line(rect.p1(), Vector(rect.get_right(), rect.get_top()),
        Color::YELLOW, LAYER_FOREGROUND1 + 11);
  }
}

namespace {

  collision::Constraints check_collisions(const Vector& obj_movement, const Rectf& moving_obj_rect, const Rectf& other_obj_rect,
    CollisionObject* moving_object = nullptr, CollisionObject* other_object = nullptr)
  {
    collision::Constraints constraints;


    // Slightly growing the static object's rectangle to detect a
    // collision not only when they overlap, but also when they're
    // adjacent or at least extremely close.
    const Rectf grown_other_obj_rect = other_obj_rect.grown(EPSILON);

    if (!moving_obj_rect.overlaps(grown_other_obj_rect))
      return constraints;

    const CollisionHit dummy;

    if (other_object != nullptr && moving_object != nullptr && !other_object->collides(*moving_object, dummy))
      return constraints;
    if (moving_object != nullptr && other_object != nullptr && !moving_object->collides(*other_object, dummy))
      return constraints;

    // Calculate intersection.
    const float itop = moving_obj_rect.get_bottom() - grown_other_obj_rect.get_top();
    const float ibottom = grown_other_obj_rect.get_bottom() - moving_obj_rect.get_top();
    const float ileft = moving_obj_rect.get_right() - grown_other_obj_rect.get_left();
    const float iright = grown_other_obj_rect.get_right() - moving_obj_rect.get_left();

    bool shiftout = false;

    if (!other_object || !other_object->is_unisolid())
    {
      if (fabsf(obj_movement.y) > fabsf(obj_movement.x)) {
        if (ileft < SHIFT_DELTA) {
          constraints.constrain_right(grown_other_obj_rect.get_left());
          shiftout = true;
        }
        else if (iright < SHIFT_DELTA) {
          constraints.constrain_left(grown_other_obj_rect.get_right());
          shiftout = true;
        }
      }
      else {
        // Shiftout bottom/top.
        if (itop < SHIFT_DELTA) {
          constraints.constrain_bottom(grown_other_obj_rect.get_top());
          shiftout = true;
        }
        else if (ibottom < SHIFT_DELTA) {
          constraints.constrain_top(grown_other_obj_rect.get_bottom());
          shiftout = true;
        }
      }
    }

    if (!shiftout)
    {
      if (other_object && other_object->is_unisolid())
      {
        // Constrain only on fall on top of the unisolid object.
        if (moving_obj_rect.get_bottom() - obj_movement.y <= grown_other_obj_rect.get_top())
        {
          constraints.constrain_bottom(other_obj_rect.get_top());
          constraints.hit.bottom = true;
        }
      }
      else
      {
        const float vert_penetration = std::min(itop, ibottom);
        const float horiz_penetration = std::min(ileft, iright);

        if (vert_penetration < horiz_penetration)
        {
          if (itop < ibottom)
          {
            constraints.constrain_bottom(grown_other_obj_rect.get_top());
            constraints.hit.bottom = true;
          }
          else
          {
            constraints.constrain_top(grown_other_obj_rect.get_bottom());
            constraints.hit.top = true;
          }
        }
        else
        {
          if (ileft < iright)
          {
            constraints.constrain_right(grown_other_obj_rect.get_left());
            constraints.hit.right = true;
          }
          else
          {
            constraints.constrain_left(grown_other_obj_rect.get_right());
            constraints.hit.left = true;
          }
        }
      }
    }

    if (other_object && moving_object)
    {
      CollisionHit hit = constraints.hit;
      moving_object->collision(*other_object, hit);
      std::swap(hit.left, hit.right);
      std::swap(hit.top, hit.bottom);
      const HitResponse& response = other_object->collision(*moving_object, hit);
      if (response == ABORT_MOVE)
        return collision::Constraints();
    }

    return constraints;
  }

} // namespace

void
CollisionSystem::collision_tilemap(collision::Constraints* constraints,
  const Vector& movement, const Rectf& dest,
  CollisionObject& object) const
{
  // calculate rectangle where the object will move
  const float x1 = dest.get_left();
  const float x2 = dest.get_right();
  const float y1 = dest.get_top();
  const float y2 = dest.get_bottom();

  for (auto* solids : m_sector.get_solid_tilemaps())
  {
    // Test with all tiles in this rectangle.
    const Rect test_tiles = solids->get_tiles_overlapping(Rectf(x1, y1, x2, y2));

    bool hits_bottom = false;

    for (int x = test_tiles.left; x < test_tiles.right; ++x)
    {
      for (int y = test_tiles.top; y < test_tiles.bottom; ++y)
      {
        const Tile& tile = solids->get_tile(x, y);

        // Skip non-solid tiles.
        if (tile.is_solid())
        {
          Rectf tile_bbox = solids->get_tile_bbox(x, y);
          bool is_relatively_solid = true;

          /* If the tile is a unisolid tile, the "is_solid()" function above
          * didn't do a thorough check. Calculate the position and (relative)
          * movement of the object and determine whether or not the tile is
          * solid with regard to those parameters. */
          if (tile.is_unisolid())
          {
            Vector relative_movement = movement
              - solids->get_movement(/* actual = */ true);

            if (!tile.is_solid(tile_bbox, object.get_bbox(), relative_movement))
              is_relatively_solid = false;
          }

          if (is_relatively_solid)
          {
            if (tile.is_slope()) { // Slope tile.
              AATriangle triangle;
              int slope_data = tile.get_data();
              if (solids->get_flip() & VERTICAL_FLIP)
                slope_data = AATriangle::vertical_flip(slope_data);
              triangle = AATriangle(tile_bbox, slope_data);

              bool triangle_hits_bottom = false;
              collision::rectangle_aatriangle(constraints, dest, triangle, triangle_hits_bottom);
              hits_bottom |= triangle_hits_bottom;
            }
            else { // Normal rectangular tile.
              collision::Constraints new_constraints = check_collisions(movement, dest, tile_bbox, nullptr, nullptr);
              hits_bottom |= new_constraints.hit.bottom;
              constraints->merge_constraints(new_constraints);
            }
          }
        }
      }
    }

    if (hits_bottom)
      solids->hits_object_bottom(object);
  }
}

uint32_t
CollisionSystem::collision_tile_attributes(const Rectf& dest, const Vector& mov) const
{
  const float x1 = dest.get_left();
  const float y1 = dest.get_top();
  const float x2 = dest.get_right();
  const float y2 = dest.get_bottom();

  uint32_t result = 0;
  for (auto& solids : m_sector.get_solid_tilemaps())
  {
    // Test with all tiles in this rectangle.
    const Rect test_tiles = solids->get_tiles_overlapping(Rectf(x1, y1, x2, y2));

    // For ice (only), add a little fudge to recognize tiles Tux is standing on.
    const Rect test_tiles_ice = solids->get_tiles_overlapping(Rectf(x1, y1, x2, y2 + SHIFT_DELTA));

    for (int x = test_tiles.left; x < test_tiles.right; ++x) {
      int y;
      for (y = test_tiles.top; y < test_tiles.bottom; ++y) {
        const Tile& tile = solids->get_tile(x, y);

        if (tile.is_collisionful(solids->get_tile_bbox(x, y), dest, mov)) {
          result |= tile.get_attributes();
        }
      }
      for (; y < test_tiles_ice.bottom; ++y) {
        const Tile& tile = solids->get_tile(x, y);
        if (tile.is_collisionful(solids->get_tile_bbox(x, y), dest, mov)) {
          result |= (tile.get_attributes() & Tile::ICE);
        }
      }
    }
  }

  return result;
}

/** Fills the CollisionHit and Normal vector between two intersecting rectangles. */
void
CollisionSystem::get_hit_normal(const CollisionObject* object1, const CollisionObject* object2,
  CollisionHit& hit, Vector& normal) const
{
  const Rectf& r1 = object1->m_dest;
  const Rectf& r2 = object2->m_dest;

  const float itop = r1.get_bottom() - r2.get_top();
  const float ibottom = r2.get_bottom() - r1.get_top();
  const float ileft = r1.get_right() - r2.get_left();
  const float iright = r2.get_right() - r1.get_left();

  const float vert_penetration = std::min(itop, ibottom);
  const float horiz_penetration = std::min(ileft, iright);

  // Apply movement only on top collision with an unisolid object.
  if (object1->is_unisolid() &&
    r2.get_bottom() - object2->m_movement.y > r1.get_top())
    return;
  if (object2->is_unisolid() &&
    r1.get_bottom() - object1->m_movement.y > r2.get_top())
    return;

  if (vert_penetration < horiz_penetration) {
    if (itop < ibottom) {
      hit.bottom = true;
      normal.y = vert_penetration;
    }
    else {
      hit.top = true;
      normal.y = -vert_penetration;
    }
  }
  else {
    if (ileft < iright) {
      hit.right = true;
      normal.x = horiz_penetration;
    }
    else {
      hit.left = true;
      normal.x = -horiz_penetration;
    }
  }
}

void
CollisionSystem::collision_object(CollisionObject* object1, CollisionObject* object2) const
{
  using namespace collision;

  // If both objects are moving statics, that means
  // their collision callbacks have already been called.
  // We don't need to call them again.
  if (object1->get_group() == COLGROUP_MOVING_STATIC &&
    object2->get_group() == COLGROUP_MOVING_STATIC)
    return;

  const Rectf& r1 = object1->m_dest;
  const Rectf& r2 = object2->m_dest;

  CollisionHit hit;
  if (r1.overlaps(r2)) {
    Vector normal(0.0f, 0.0f);
    get_hit_normal(object1, object2, hit, normal);

    if (!object1->collides(*object2, hit))
      return;
    std::swap(hit.left, hit.right);
    std::swap(hit.top, hit.bottom);
    if (!object2->collides(*object1, hit))
      return;
    std::swap(hit.left, hit.right);
    std::swap(hit.top, hit.bottom);

    HitResponse response1 = object1->collision(*object2, hit);
    std::swap(hit.left, hit.right);
    std::swap(hit.top, hit.bottom);
    HitResponse response2 = object2->collision(*object1, hit);
    if (response1 == CONTINUE && response2 == CONTINUE) {
      normal *= (0.5f + EPSILON);
      object1->m_dest.move(-normal);
      object2->m_dest.move(normal);
    }
    else if (response1 == CONTINUE && response2 == FORCE_MOVE) {
      normal *= (1 + EPSILON);
      object1->m_dest.move(-normal);
    }
    else if (response1 == FORCE_MOVE && response2 == CONTINUE) {
      normal *= (1 + EPSILON);
      object2->m_dest.move(normal);
    }
  }
}

void
CollisionSystem::collision_static(collision::Constraints* constraints,
  const Vector& movement, const Rectf& dest,
  CollisionObject& object)
{
  collision_tilemap(constraints, movement, dest, object);

  // Collision with other (static) objects.
  for (auto* static_object : m_objects)
  {
    if ((
      static_object->get_group() == COLGROUP_STATIC ||
      static_object->get_group() == COLGROUP_MOVING_STATIC
      ) &&
      static_object->is_valid() &&
      static_object != &object)
    {

      collision::Constraints new_constraints = check_collisions(
        movement, dest, static_object->m_dest, &object, static_object);

      if (new_constraints.hit.bottom)
        static_object->collision_moving_object_bottom(object);
      else if (new_constraints.hit.top)
        object.collision_moving_object_bottom(*static_object);

      constraints->merge_constraints(new_constraints);
    }
  }
}

void
CollisionSystem::collision_static_constrains(CollisionObject& object)
{
  using namespace collision;

  const float infinity = (std::numeric_limits<float>::has_infinity ? std::numeric_limits<float>::infinity() : std::numeric_limits<float>::max());

  Constraints constraints;
  const Vector movement = object.get_movement();
  Vector pressure = Vector(0, 0);
  Rectf& dest = object.m_dest;

  for (int i = 0; i < 2; ++i) {
    collision_static(&constraints, Vector(0, movement.y), dest, object);
    if (!constraints.has_constraints())
      break;

    // Apply calculated horizontal constraints.
    if (constraints.get_position_bottom() < infinity) {
      float height = constraints.get_height();
      if (height < object.get_bbox().get_height()) {
        // We're crushed, but ignore this for now, we'll get this again
        // later if we're really crushed or things will solve itself when
        // looking at the vertical constraints.
        pressure.y += object.get_bbox().get_height() - height;
        object.m_pressure.y = pressure.y;
      }
      else {
        dest.set_bottom(constraints.get_position_bottom() - EPSILON);
        dest.set_top(dest.get_bottom() - object.get_bbox().get_height());
      }
    }
    else if (constraints.get_position_top() > -infinity) {
      dest.set_top(constraints.get_position_top() + EPSILON);
      dest.set_bottom(dest.get_top() + object.get_bbox().get_height());
    }
  }

  if (constraints.has_constraints()) {
    if (constraints.hit.top || constraints.hit.bottom) {
      constraints.hit.left = false;
      constraints.hit.right = false;
      object.collision_solid(constraints.hit);
    }
  }

  constraints = Constraints();
  for (int i = 0; i < 2; ++i) {
    collision_static(&constraints, movement, dest, object);
    if (!constraints.has_constraints())
      break;

    // Apply calculated vertical constraints.
    const float width = constraints.get_width();

    if (width < infinity) {
      if (width + SHIFT_DELTA < object.get_bbox().get_width()) {
        // We're crushed, but ignore this for now, we'll get this again
        // later if we're really crushed or things will solve itself when
        // looking at the horizontal constraints.
        pressure.x += object.get_bbox().get_width() - width;
        object.m_pressure.x = pressure.x;
      }
      else {
        float xmid = constraints.get_x_midpoint();
        dest.set_left(xmid - object.get_bbox().get_width() / 2);
        dest.set_right(xmid + object.get_bbox().get_width() / 2);
      }
    }
    else if (constraints.get_position_right() < infinity) {
      dest.set_right(constraints.get_position_right() - EPSILON);
      dest.set_left(dest.get_right() - object.get_bbox().get_width());
    }
    else if (constraints.get_position_left() > -infinity) {
      dest.set_left(constraints.get_position_left() + EPSILON);
      dest.set_right(dest.get_left() + object.get_bbox().get_width());
    }
  }

  if (constraints.has_constraints()) {
    if (constraints.hit.left || constraints.hit.right
      || constraints.hit.top || constraints.hit.bottom
      || constraints.hit.crush)
      object.collision_solid(constraints.hit);
  }

  // An extra pass to make sure we're not crushed vertically.
  if (pressure.y > 0) {
    constraints = Constraints();
    collision_static(&constraints, movement, dest, object);
    if (constraints.get_position_bottom() < infinity) {
      const float height = constraints.get_height();

      if (height + SHIFT_DELTA < object.get_bbox().get_height()) {
        CollisionHit h;
        h.top = true;
        h.bottom = true;
        h.crush = pressure.y > 16;
        object.collision_solid(h);
      }
    }
  }

  // An extra pass to make sure we're not crushed horizontally.
  if (pressure.x > 0) {
    constraints = Constraints();
    collision_static(&constraints, movement, dest, object);
    if (constraints.get_position_right() < infinity) {
      float width = constraints.get_width();
      if (width + SHIFT_DELTA < object.get_bbox().get_width()) {
        CollisionHit h;
        h.top = true;
        h.bottom = true;
        h.left = true;
        h.right = true;
        h.crush = pressure.x > 16;
        object.collision_solid(h);
      }
    }
  }
}

void
CollisionSystem::update()
{
  if (Editor::is_active()) {
    return;
    // Objects in editor shouldn't collide.
  }

  using namespace collision;

  m_ground_movement_manager->apply_all_ground_movement();

  // Calculate destination positions of the objects.
  for (const auto& object : m_objects)
  {
    const Vector& mov = object->get_movement();

    // Make sure movement is never faster than MAX_SPEED.
    if (glm::length(mov) > MAX_SPEED) {
      object->m_movement = glm::normalize(mov) * MAX_SPEED;
    }

    object->m_dest = object->get_bbox();
    object->m_pressure = Vector(0, 0);
    object->m_dest.move(object->get_movement());
    object->clear_bottom_collision_list();
  }

  // Part 1: COLGROUP_MOVING vs COLGROUP_STATIC and tilemap.
  for (const auto& object : m_objects) {
    if ((object->get_group() != COLGROUP_MOVING
      && object->get_group() != COLGROUP_MOVING_STATIC
      && object->get_group() != COLGROUP_MOVING_ONLY_STATIC)
      || !object->is_valid())
      continue;

    collision_static_constrains(*object);
  }

  // Part 2: COLGROUP_MOVING vs tile attributes.
  for (const auto& object : m_objects) {
    if ((object->get_group() != COLGROUP_MOVING
      && object->get_group() != COLGROUP_MOVING_STATIC
      && object->get_group() != COLGROUP_MOVING_ONLY_STATIC)
      || !object->is_valid())
      continue;

    uint32_t tile_attributes = collision_tile_attributes(object->m_dest, object->get_movement());
    if (tile_attributes >= Tile::FIRST_INTERESTING_FLAG) {
      object->collision_tile(tile_attributes);
    }
  }

  // Part 2.5: COLGROUP_MOVING vs COLGROUP_TOUCHABLE.
  for (const auto& object : m_objects)
  {
    if ((object->get_group() != COLGROUP_MOVING
      && object->get_group() != COLGROUP_MOVING_STATIC)
      || !object->is_valid())
      continue;

    for (auto& object_2 : m_objects) {
      if (object_2->get_group() != COLGROUP_TOUCHABLE
        || !object_2->is_valid())
        continue;

      if (object->m_dest.overlaps(object_2->m_dest)) {
        Vector normal(0.0f, 0.0f);
        CollisionHit hit;
        get_hit_normal(object, object_2, hit, normal);
        if (!object->collides(*object_2, hit))
          continue;
        if (!object_2->collides(*object, hit))
          continue;

        object->collision(*object_2, hit);
        object_2->collision(*object, hit);
      }
    }
  }

  // Part 3: COLGROUP_MOVING vs COLGROUP_MOVING.
  for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
  {
    auto object = *i;

    if (!object->is_valid() ||
      (object->get_group() != COLGROUP_MOVING &&
        object->get_group() != COLGROUP_MOVING_STATIC))
      continue;

    for (auto i2 = i + 1; i2 != m_objects.end(); ++i2) {
      auto object_2 = *i2;
      if ((object_2->get_group() != COLGROUP_MOVING
        && object_2->get_group() != COLGROUP_MOVING_STATIC)
        || !object_2->is_valid())
        continue;

      collision_object(object, object_2);
    }
  }

  // Apply object movement.
  for (auto* object : m_objects) {
    object->m_bbox = object->m_dest;
    object->m_movement = Vector(0, 0);
  }
}

bool
CollisionSystem::is_free_of_tiles(const Rectf& rect, const bool ignoreUnisolid, uint32_t tiletype) const
{
  using namespace collision;

  for (const auto& solids : m_sector.get_solid_tilemaps()) {
    // Test with all tiles in this rectangle.
    const Rect test_tiles = solids->get_tiles_overlapping(rect);

    for (int x = test_tiles.left; x < test_tiles.right; ++x) {
      for (int y = test_tiles.top; y < test_tiles.bottom; ++y) {
        const Tile& tile = solids->get_tile(x, y);

        if (!(tile.get_attributes() & tiletype))
          continue;
        if (tile.is_unisolid() && ignoreUnisolid)
          continue;
        if (tile.is_slope()) {
          AATriangle triangle;
          const Rectf tbbox = solids->get_tile_bbox(x, y);
          triangle = AATriangle(tbbox, tile.get_data());
          Constraints constraints;
          if (!collision::rectangle_aatriangle(&constraints, rect, triangle))
            continue;
        }
        // We have a solid tile that overlaps the given rectangle.
        return false;
      }
    }
  }

  return true;
}

bool
CollisionSystem::is_free_of_statics(const Rectf& rect, const CollisionObject* ignore_object, const bool ignoreUnisolid, uint32_t tiletype) const
{
  using namespace collision;

  if (!is_free_of_tiles(rect, ignoreUnisolid, tiletype)) return false;

  for (const auto& object : m_objects) {
    if (object == ignore_object) continue;
    if (!object->is_valid()) continue;
    if (object->get_group() == COLGROUP_STATIC) {
      if (rect.overlaps(object->get_bbox())) return false;
    }
  }

  return true;
}

bool
CollisionSystem::is_free_of_movingstatics(const Rectf& rect, const CollisionObject* ignore_object, const bool ignore_unisolid) const
{
  using namespace collision;

  if (!is_free_of_tiles(rect, ignore_unisolid)) return false;

  for (const auto& object : m_objects) {
    if (object == ignore_object) continue;
    if (!object->is_valid()) continue;
    if ((object->get_group() == COLGROUP_MOVING)
      || (object->get_group() == COLGROUP_MOVING_STATIC)
      || (object->get_group() == COLGROUP_STATIC)) {
      if (rect.overlaps(object->get_bbox())) return false;
    }
  }

  return true;
}

bool
CollisionSystem::is_free_of_specifically_movingstatics(const Rectf& rect, const CollisionObject* ignore_object) const
{
  using namespace collision;

  for (const auto& object : m_objects) {
    if (object == ignore_object) continue;
    if (!object->is_valid()) continue;
    if ((object->get_group() == COLGROUP_MOVING_STATIC)
      && (rect.overlaps(object->get_bbox())))
      return false;
  }

  return true;
}

CollisionSystem::RaycastResult
CollisionSystem::get_first_line_intersection(const Vector& line_start,
  const Vector& line_end,
  RaycastIgnore ignore,
  const CollisionObject* ignore_object) const
{
  using namespace collision;
  RaycastResult tileresult;

  if (ignore != IGNORE_TILES)
  {
    // Check if no tile is in the way.
    const float lsx = std::min(line_start.x, line_end.x);
    const float lex = std::max(line_start.x, line_end.x);
    const float lsy = std::min(line_start.y, line_end.y);
    const float ley = std::max(line_start.y, line_end.y);

    for (float test_x = lsx; test_x <= lex; test_x += 16) { // NOLINT.
      for (float test_y = lsy; test_y <= ley; test_y += 16) { // NOLINT.
        for (const auto& solids : m_sector.get_solid_tilemaps()) {
          const auto& test_vector = Vector(test_x, test_y);
          if (solids->is_outside_bounds(test_vector))
          {
            continue;
          }

          const Tile* tile = &solids->get_tile_at(test_vector);

          // FIXME: check collision with slope tiles
          if ((tile->get_attributes() & Tile::SOLID))
          {
            tileresult.is_valid = true;
            tileresult.hit = tile;
            tileresult.box = solids->get_tile_bbox(static_cast<int>(test_vector.x / 32.f), static_cast<int>(test_vector.y / 32.f));
            goto finish_tiles;
          }
        }
      }
    }
  }

finish_tiles:
  if (ignore == IGNORE_OBJECTS)
    return tileresult;

  RaycastResult objresult;

  // Check if no object is in the way.
  for (const auto& object : m_objects) {
    if (object == ignore_object) continue;
    if (!object->is_valid()) continue;
    if ((object->get_group() == COLGROUP_MOVING)
      || (object->get_group() == COLGROUP_MOVING_STATIC)
      || (object->get_group() == COLGROUP_STATIC))
    {
      if (intersects_line(object->get_bbox(), line_start, line_end))
      {
        objresult.is_valid = true;
        objresult.hit = object;
        objresult.box = object->get_bbox();
        break;
      }
    }
  }

  if (ignore == IGNORE_TILES)
    return objresult;

  if (tileresult.is_valid && objresult.is_valid)
  {
    float tiledist = glm::distance(tileresult.box.get_middle(), line_start);
    float objdist = glm::distance(objresult.box.get_middle(), line_start);
    return tiledist < objdist ? tileresult : objresult;
  }
  else if (tileresult.is_valid)
    return tileresult;
  else if (objresult.is_valid)
    return objresult;
  else
  {
    return RaycastResult();
  }
}

bool
CollisionSystem::free_line_of_sight(const Vector& line_start, const Vector& line_end, bool ignore_objects, const CollisionObject* ignore_object) const
{
  auto ignore = (ignore_objects ? IGNORE_OBJECTS : IGNORE_NONE);
  return !get_first_line_intersection(line_start, line_end, ignore, ignore_object).is_valid;
}

std::vector<CollisionObject*>
CollisionSystem::get_nearby_objects(const Vector& center, float max_distance) const
{
  std::vector<CollisionObject*> ret;

  for (const auto& object : m_objects) {
    float distance = object->get_bbox().distance(center);
    if (distance <= max_distance)
      ret.push_back(object);
  }

  return ret;
}

/* EOF */
