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

#ifndef HEADER_SUPERTUX_COLLISION_COLLISION_HPP
#define HEADER_SUPERTUX_COLLISION_COLLISION_HPP

#include <limits>
#include <algorithm>

#include "collision/collision_hit.hpp"
#include "math/fwd.hpp"

class Rectf;
class AATriangle;

namespace collision {

class Constraints final
{
public:
  Constraints() :
    hit(),
    position_left(),
    position_right(),
    position_top(),
    position_bottom()
  {
    float infinity = (std::numeric_limits<float>::has_infinity ?
                      std::numeric_limits<float>::infinity() :
                      std::numeric_limits<float>::max());
    position_left = -infinity;
    position_right = infinity;
    position_top = -infinity;
    position_bottom = infinity;
  }

  bool has_constraints() const
  {
    float infinity = (std::numeric_limits<float>::has_infinity ?
                      std::numeric_limits<float>::infinity() :
                      std::numeric_limits<float>::max());
    return
      position_left   > -infinity ||
      position_right  <  infinity ||
      position_top    > -infinity ||
      position_bottom <  infinity;
  }

public:

  void constrain_left (float position)
  {
    position_left = std::max (position_left, position);
  }

  void constrain_right (float position)
  {
    position_right = std::min (position_right, position);
  }

  void constrain_top (float position)
  {
    position_top = std::max (position_top, position);
  }

  void constrain_bottom (float position)
  {
    position_bottom = std::min (position_bottom, position);
  }

  void merge_constraints (const Constraints& other);

  float get_position_left   () const { return position_left;   }
  float get_position_right  () const { return position_right;  }
  float get_position_top    () const { return position_top;    }
  inline float get_position_bottom () const { return position_bottom; }

  inline float get_height () const { return (position_bottom - position_top); }
  inline float get_width  () const { return (position_right - position_left); }

  inline float get_x_midpoint () const { return (.5f * (position_left + position_right)); }

  CollisionHit hit;

private:
  float position_left;
  float position_right;
  float position_top;
  float position_bottom;
};

/** does collision detection between a rectangle and an axis aligned triangle
 * Returns true in case of a collision and fills in the hit structure then.
 */
bool rectangle_aatriangle(Constraints* constraints, const Rectf& rect,
                          const AATriangle& triangle);

bool rectangle_aatriangle(Constraints* constraints, const Rectf& rect,
                          const AATriangle& triangle,
                          bool& hits_rectangle_bottom);

void set_rectangle_rectangle_constraints(Constraints* constraints, const Rectf& r1, const Rectf& r2);

bool line_intersects_line(const Vector& line1_start, const Vector& line1_end, const Vector& line2_start, const Vector& line2_end);
bool intersects_line(const Rectf& r, const Vector& line_start, const Vector& line_end);

} // namespace collision

#endif

/* EOF */
