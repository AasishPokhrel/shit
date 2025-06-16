//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#include "supertux/tile.hpp"

#include "math/aatriangle.hpp"
#include "supertux/constants.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

bool Tile::draw_editor_images = false;

namespace {

bool is_above_line (float l_x, float l_y, float m,
                    float p_x, float p_y)
{
  float interp_y = (l_y + (m * (p_x - l_x)));
  return (interp_y >= p_y);
}

bool is_below_line (float l_x, float l_y, float m,
                    float p_x, float p_y)
{
  return !is_above_line (l_x, l_y, m, p_x, p_y);
}

} // namespace

Tile::Tile() :
  m_images(),
  m_editor_images(),
  m_attributes(0),
  m_data(0),
  m_fps(1),
  m_object_name(),
  m_object_data(),
  m_deprecated(false)
{
}

Tile::Tile(const std::vector<SurfacePtr>& images,
           const std::vector<SurfacePtr>& editor_images,
           uint32_t attributes, uint32_t data, float fps,
           bool deprecated,
           const std::string& obj_name,
           const std::string& obj_data) :
  m_images(images),
  m_editor_images(editor_images),
  m_attributes(attributes),
  m_data(data),
  m_fps(fps),
  m_object_name(obj_name),
  m_object_data(obj_data),
  m_deprecated(deprecated)
{
}

void
Tile::draw(Canvas& canvas, const Vector& pos, int z_pos, const Color& color) const
{
  if (draw_editor_images && m_editor_images.size() > 0) {
    size_t frame_no = 0;
    if (m_editor_images.size() > 1) {
      frame_no = size_t(g_game_time * m_fps) % m_editor_images.size();
    }
    canvas.draw_surface(m_editor_images[frame_no], pos, 0, color, Blend(), z_pos);
    return;
  }

  if(m_images.size() > 0)
  {
    size_t frame_no = 0;
    if (m_images.size() > 1) {
      frame_no = size_t(g_game_time * m_fps) % m_images.size();
    }
    canvas.draw_surface(m_images[frame_no], pos, 0, color, Blend(), z_pos);
  }
}

void
Tile::draw_debug(Canvas& canvas, const Vector& pos, int z_pos, const Color& color) const
{
  if (!is_slope())
  {
    if (m_attributes & SOLID) {
      canvas.draw_filled_rect(Rectf(pos, Sizef(32.0f, 32.0f)), color, z_pos);
    }
  }
  else
  {
    int slope_info = get_data();

    switch (slope_info)
    {
      case AATriangle::SOUTHWEST:
        canvas.draw_triangle({pos.x, pos.y}, {pos.x, pos.y + 32.0f}, {pos.x + 32.0f, pos.y + 32.0f}, color, z_pos);
        break;

      case AATriangle::NORTHEAST:
        canvas.draw_triangle({pos.x, pos.y}, {pos.x + 32.0f, pos.y}, {pos.x + 32.0f, pos.y + 32.0f}, color, z_pos);
        break;

      case AATriangle::SOUTHEAST:
        canvas.draw_triangle({pos.x, pos.y + 32.0f}, {pos.x + 32.0f, pos.y}, {pos.x + 32.0f, pos.y + 32.0f}, color, z_pos);
        break;

      case AATriangle::NORTHWEST:
        canvas.draw_triangle({pos.x, pos.y}, {pos.x + 32.0f, pos.y}, {pos.x, pos.y + 32.0f}, color, z_pos);
        break;


      case AATriangle::SOUTHWEST | AATriangle::DEFORM_BOTTOM:
        canvas.draw_triangle({pos.x, pos.y + 16.0f}, {pos.x, pos.y + 32.0f}, {pos.x + 32.0f, pos.y + 32.0f}, color, z_pos);
        break;

      case AATriangle::NORTHEAST | AATriangle::DEFORM_BOTTOM:
        canvas.draw_filled_rect(Rectf(pos.x, pos.y, pos.x + 32.0f, pos.y + 16.0f), color, z_pos);
        canvas.draw_triangle({pos.x, pos.y + 16.0f}, {pos.x + 32.0f, pos.y + 16.0f}, {pos.x + 32.0f, pos.y + 32.0f}, color, z_pos);
        break;

      case AATriangle::SOUTHEAST | AATriangle::DEFORM_BOTTOM:
        canvas.draw_triangle({pos.x + 32.0f, pos.y + 32.0f}, {pos.x, pos.y + 32.0f}, {pos.x + 32.0f, pos.y + 16.0f}, color, z_pos);
        break;

      case AATriangle::NORTHWEST | AATriangle::DEFORM_BOTTOM:
        canvas.draw_filled_rect(Rectf(pos.x, pos.y, pos.x + 32.0f, pos.y + 16.0f), color, z_pos);
        canvas.draw_triangle({pos.x, pos.y + 32.0f}, {pos.x + 32.0f, pos.y + 16.0f}, {pos.x, pos.y + 16.0f}, color, z_pos);
        break;


      case AATriangle::SOUTHWEST | AATriangle::DEFORM_TOP:
        canvas.draw_filled_rect(Rectf(pos.x, pos.y + 16.0f, pos.x + 32.0f, pos.y + 32.0f), color, z_pos);
        canvas.draw_triangle({pos.x, pos.y + 16.0f}, {pos.x + 32.0f, pos.y + 16.0f}, {pos.x, pos.y}, color, z_pos);
        break;

      case AATriangle::NORTHEAST | AATriangle::DEFORM_TOP:
        canvas.draw_triangle({pos.x + 32.0f, pos.y}, {pos.x + 32.0f, pos.y + 16.0f}, {pos.x, pos.y}, color, z_pos);
        break;

      case AATriangle::SOUTHEAST | AATriangle::DEFORM_TOP:
        canvas.draw_filled_rect(Rectf(pos.x, pos.y + 16.0f, pos.x + 32.0f, pos.y + 32.0f), color, z_pos);
        canvas.draw_triangle({pos.x + 32.0f, pos.y + 16.0f}, {pos.x, pos.y + 16.0f}, {pos.x + 32.0f, pos.y}, color, z_pos);
        break;

      case AATriangle::NORTHWEST | AATriangle::DEFORM_TOP:
        canvas.draw_triangle({pos.x, pos.y}, {pos.x + 32.0f, pos.y}, {pos.x, pos.y + 16.0f}, color, z_pos);
        break;


      case AATriangle::SOUTHWEST | AATriangle::DEFORM_LEFT:
        canvas.draw_triangle({pos.x, pos.y + 32.0f}, {pos.x, pos.y}, {pos.x + 16.0f, pos.y + 32.0f}, color, z_pos);
        break;

      case AATriangle::NORTHEAST | AATriangle::DEFORM_LEFT:
        canvas.draw_filled_rect(Rectf(pos.x + 16.0f, pos.y, pos.x + 32.0f, pos.y + 32.0f), color, z_pos);
        canvas.draw_triangle({pos.x + 16.0f, pos.y}, {pos.x + 16.0f, pos.y + 32.0f}, {pos.x, pos.y}, color, z_pos);
        break;

      case AATriangle::SOUTHEAST | AATriangle::DEFORM_LEFT:
        canvas.draw_filled_rect(Rectf(pos.x + 16.0f, pos.y, pos.x + 32.0f, pos.y + 32.0f), color, z_pos);
        canvas.draw_triangle({pos.x + 16.0f, pos.y + 32.0f}, {pos.x, pos.y + 32.0f}, {pos.x + 16.0f, pos.y}, color, z_pos);
        break;

      case AATriangle::NORTHWEST | AATriangle::DEFORM_LEFT:
        canvas.draw_triangle({pos.x, pos.y}, {pos.x + 16.0f, pos.y}, {pos.x, pos.y + 16.0f}, color, z_pos);
        break;


      case AATriangle::SOUTHWEST | AATriangle::DEFORM_RIGHT:
        canvas.draw_filled_rect(Rectf(pos.x, pos.y, pos.x + 16.0f, pos.y + 32.0f), color, z_pos);
        canvas.draw_triangle({pos.x + 16.0f, pos.y + 32.0f}, {pos.x + 16.0f, pos.y}, {pos.x + 32.0f, pos.y + 32.0f}, color, z_pos);
        break;

      case AATriangle::NORTHEAST | AATriangle::DEFORM_RIGHT:
        canvas.draw_triangle({pos.x + 32.0f, pos.y}, {pos.x + 32.0f, pos.y + 32.0f}, {pos.x + 16.0f, pos.y}, color, z_pos);
        break;

      case AATriangle::SOUTHEAST | AATriangle::DEFORM_RIGHT:
        canvas.draw_triangle({pos.x + 32.0f, pos.y + 32.0f}, {pos.x + 16.0f, pos.y + 32.0f}, {pos.x + 32.0f, pos.y}, color, z_pos);
        break;

      case AATriangle::NORTHWEST | AATriangle::DEFORM_RIGHT:
        canvas.draw_filled_rect(Rectf(pos.x, pos.y, pos.x + 16.0f, pos.y + 32.0f), color, z_pos);
        canvas.draw_triangle({pos.x + 16.0f, pos.y}, {pos.x + 32.0f, pos.y}, {pos.x + 16.0f, pos.y + 32.0f}, color, z_pos);
        break;
    }
  }
}

SurfacePtr
Tile::get_current_surface() const
{
  if (m_images.size() > 1) {
    size_t frame = size_t(g_game_time * m_fps) % m_images.size();
    return m_images[frame];
  } else if (m_images.size() == 1) {
    return m_images[0];
  } else {
    return {};
  }
}

SurfacePtr
Tile::get_current_editor_surface() const
{
  if (m_editor_images.size() > 1) {
    size_t frame = size_t(g_game_time * m_fps) % m_editor_images.size();
    return m_editor_images[frame];
  } else if (m_editor_images.size() == 1) {
    return m_editor_images[0];
  } else {
    return get_current_surface();
  }
}

// Check if the tile is solid given the current movement. This works
// for south-slopes (which are solid when moving "down") and
// north-slopes (which are solid when moving "up". "up" and "down" is
// in quotation marks because because the slope's gradient is taken.
// Also, this uses the movement relative to the tilemaps own movement
// (if any).  --octo
bool
Tile::check_movement_unisolid (const Vector& movement) const
{
  int slope_info;
  double mv_x;
  double mv_y;
  double mv_tan;
  double slope_tan;

  //If the tile is not a slope, this is very easy.
  if (!is_slope())
  {
    int dir = get_data() & Tile::UNI_DIR_MASK;

    return ((dir == Tile::UNI_DIR_NORTH) && (movement.y > -EPSILON)) /* moving down */
        || ((dir == Tile::UNI_DIR_SOUTH) && (movement.y < EPSILON))  /* moving up */
        || ((dir == Tile::UNI_DIR_WEST ) && (movement.x > -EPSILON)) /* moving right */
        || ((dir == Tile::UNI_DIR_EAST ) && (movement.x < EPSILON)); /* moving left */
  }

  // Initialize mv_x and mv_y. Depending on the slope the axis are inverted so
  // that we can always use the "SOUTHEAST" case of the slope. The southeast
  // case is the following:
  //     .
  //    /!
  //   / !
  //  +--+
  mv_x = static_cast<double>(movement.x); // note switch to double for no good reason
  mv_y = static_cast<double>(movement.y);

  slope_info = get_data();
  switch (slope_info & AATriangle::DIRECTION_MASK)
  {
    case AATriangle::SOUTHEAST: /*    . */
      /* do nothing */          /*   /! */
      break;                    /*  / ! */
                                /* +--+ */
    case AATriangle::SOUTHWEST: /* .    */
      mv_x *= (-1.0);           /* !\   */
      break;                    /* ! \  */
                                /* +--+ */
    case AATriangle::NORTHEAST: /* +--+ */
      mv_y *= (-1.0);           /*  \ ! */
      break;                    /*   \! */
                                /*    ' */
    case AATriangle::NORTHWEST: /* +--+ */
      mv_x *= (-1.0);           /* ! /  */
      mv_y *= (-1.0);           /* !/   */
      break;                    /* '    */
  }

  // Handle the easy cases first
  // If we're moving to the right and down, then the slope is solid.
  if ((mv_x >= 0.0) && (mv_y >= 0.0)) // 4th quadrant
    return true;
  // If we're moving to the left and up, then the slope is not solid.
  else if ((mv_x <= 0.0) && (mv_y <= 0.0)) // 2nd quadrant
    return false;

  // The pure up-down and left-right movements have already been handled.
  assert (mv_x != 0.0);
  assert (mv_y != 0.0);

  // calculate tangent of movement
  mv_tan = (-1.0) * mv_y / mv_x;

  // determine tangent of the slope
  slope_tan = 1.0;
  if (((slope_info & AATriangle::DEFORM_MASK) == AATriangle::DEFORM_BOTTOM)
      || ((slope_info & AATriangle::DEFORM_MASK) == AATriangle::DEFORM_TOP))
    slope_tan = 0.5; // ~= 26.6 deg
  else if (((slope_info & AATriangle::DEFORM_MASK) == AATriangle::DEFORM_LEFT)
      || ((slope_info & AATriangle::DEFORM_MASK) == AATriangle::DEFORM_RIGHT))
    slope_tan = 2.0; // ~= 63.4 deg

  // up and right
  if (mv_x > 0.0) // 1st quadrant
  {
    assert (mv_y < 0.0);
    return (mv_tan <= slope_tan);
  }
  // down and left
  else if (mv_x < 0.0) // 3rd quadrant
  {
    assert (mv_y > 0.0);
    return (mv_tan >= slope_tan);
  }

  return false;
}

// Check whether the object is already *in* the tile. If so, the tile
// is non-solid. Otherwise, if the object is "above" (south slopes) or
// "below" (north slopes), the tile will be solid.
bool
Tile::check_position_unisolid (const Rectf& obj_bbox,
                               const Rectf& tile_bbox) const
{
  int slope_info;
  float tile_x;
  float tile_y;
  float gradient;
  float delta_x;
  float delta_y;
  float obj_x = 0.0;
  float obj_y = 0.0;

  // If this is not a slope, this is - again - easy
  if (!is_slope())
  {
    int dir = get_data() & Tile::UNI_DIR_MASK;

    return ((dir == Tile::UNI_DIR_NORTH) && ((obj_bbox.get_bottom() - SHIFT_DELTA) <= tile_bbox.get_top()   ))
        || ((dir == Tile::UNI_DIR_SOUTH) && ((obj_bbox.get_top()    + SHIFT_DELTA) >= tile_bbox.get_bottom()))
        || ((dir == Tile::UNI_DIR_WEST ) && ((obj_bbox.get_right()  - SHIFT_DELTA) <= tile_bbox.get_left()  ))
        || ((dir == Tile::UNI_DIR_EAST ) && ((obj_bbox.get_left()   + SHIFT_DELTA) >= tile_bbox.get_right() ));
  }

  // There are 20 different cases. For each case, calculate a line
  // that describes the slope's surface. The line is defined by x, y,
  // and m, the gradient.
  slope_info = get_data();
  switch (slope_info
      & (AATriangle::DIRECTION_MASK | AATriangle::DEFORM_MASK))
  {
    case AATriangle::SOUTHWEST:
    case AATriangle::SOUTHWEST | AATriangle::DEFORM_TOP:
    case AATriangle::SOUTHWEST | AATriangle::DEFORM_LEFT:
    case AATriangle::NORTHEAST:
    case AATriangle::NORTHEAST | AATriangle::DEFORM_TOP:
    case AATriangle::NORTHEAST | AATriangle::DEFORM_LEFT:
      tile_x = tile_bbox.get_left();
      tile_y = tile_bbox.get_top();
      gradient = 1.0;
      break;

    case AATriangle::SOUTHEAST:
    case AATriangle::SOUTHEAST | AATriangle::DEFORM_TOP:
    case AATriangle::SOUTHEAST | AATriangle::DEFORM_RIGHT:
    case AATriangle::NORTHWEST:
    case AATriangle::NORTHWEST | AATriangle::DEFORM_TOP:
    case AATriangle::NORTHWEST | AATriangle::DEFORM_RIGHT:
      tile_x = tile_bbox.get_right();
      tile_y = tile_bbox.get_top();
      gradient = -1.0;
      break;

    case AATriangle::SOUTHEAST | AATriangle::DEFORM_BOTTOM:
    case AATriangle::SOUTHEAST | AATriangle::DEFORM_LEFT:
    case AATriangle::NORTHWEST | AATriangle::DEFORM_BOTTOM:
    case AATriangle::NORTHWEST | AATriangle::DEFORM_LEFT:
      tile_x = tile_bbox.get_left();
      tile_y = tile_bbox.get_bottom();
      gradient = -1.0;
      break;

    case AATriangle::SOUTHWEST | AATriangle::DEFORM_BOTTOM:
    case AATriangle::SOUTHWEST | AATriangle::DEFORM_RIGHT:
    case AATriangle::NORTHEAST | AATriangle::DEFORM_BOTTOM:
    case AATriangle::NORTHEAST | AATriangle::DEFORM_RIGHT:
      tile_x = tile_bbox.get_right();
      tile_y = tile_bbox.get_bottom();
      gradient = 1.0;
      break;

    default:
      assert(false);
      return 0;
  }

  // delta_x, delta_y: Gradient aware version of SHIFT_DELTA. Here, we set the
  // sign of the values only. Also, we determine here which corner of the
  // object's bounding box is the interesting one for us.
  delta_x = 1.0f * SHIFT_DELTA;
  delta_y = 1.0f * SHIFT_DELTA;
  switch (slope_info & AATriangle::DIRECTION_MASK)
  {
    case AATriangle::SOUTHWEST:
      delta_x *= 1.f;
      delta_y *= -1.f;
      obj_x = obj_bbox.get_left();
      obj_y = obj_bbox.get_bottom();
      break;

    case AATriangle::SOUTHEAST:
      delta_x *= -1.f;
      delta_y *= -1.f;
      obj_x = obj_bbox.get_right();
      obj_y = obj_bbox.get_bottom();
      break;

    case AATriangle::NORTHWEST:
      delta_x *= 1.f;
      delta_y *= 1.f;
      obj_x = obj_bbox.get_left();
      obj_y = obj_bbox.get_top();
      break;

    case AATriangle::NORTHEAST:
      delta_x *= -1.f;
      delta_y *= 1.f;
      obj_x = obj_bbox.get_right();
      obj_y = obj_bbox.get_top();
      break;
  }

  // Adapt the delta_x, delta_y and the gradient for the 26.6 deg and
  // 63.4 deg cases.
  switch (slope_info & AATriangle::DEFORM_MASK)
  {
    case 0:
      delta_x *= .70710678118654752440f; // 1/sqrt(2)
      delta_y *= .70710678118654752440f; // 1/sqrt(2)
      break;

    case AATriangle::DEFORM_BOTTOM:
    case AATriangle::DEFORM_TOP:
      delta_x *= .44721359549995793928f; // 1/sqrt(5)
      delta_y *= .89442719099991587856f; // 2/sqrt(5)
      gradient *= 0.5f;
      break;

    case AATriangle::DEFORM_LEFT:
    case AATriangle::DEFORM_RIGHT:
      delta_x *= .89442719099991587856f; // 2/sqrt(5)
      delta_y *= .44721359549995793928f; // 1/sqrt(5)
      gradient *= 2.f;
      break;
  }

  // With a south slope, check if all points are above the line. If
  // one point isn't, the slope is not solid. => You can pass through
  // a south-slope from below but not from above.
  if (((slope_info & AATriangle::DIRECTION_MASK) == AATriangle::SOUTHWEST) ||
      ((slope_info & AATriangle::DIRECTION_MASK) == AATriangle::SOUTHEAST))
  {
    return !is_below_line(tile_x, tile_y, gradient, obj_x + delta_x, obj_y + delta_y);
  }
  // northwest or northeast. Same as above, but inverted. You can pass
  // from top to bottom but not vice versa.
  else
  {
    return !is_above_line (tile_x, tile_y, gradient, obj_x + delta_x, obj_y + delta_y);
  }
}

bool
Tile::is_solid (const Rectf& tile_bbox, const Rectf& position, const Vector& movement) const
{
  if (!(m_attributes & SOLID))
    return false;

  return is_collisionful(tile_bbox, position, movement);
}

bool
Tile::is_collisionful(const Rectf& tile_bbox, const Rectf& position, const Vector& movement) const
{
  if (!(m_attributes & UNISOLID))
    return true;

  return check_movement_unisolid (movement) &&
         check_position_unisolid (position, tile_bbox);
}

/* EOF */
