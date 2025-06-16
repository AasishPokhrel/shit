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

#ifndef HEADER_SUPERTUX_MATH_AATRIANGLE_HPP
#define HEADER_SUPERTUX_MATH_AATRIANGLE_HPP

#include "math/rectf.hpp"

/**
 * An axis-aligned triangle (ie. a triangle where 2 sides are parallel to the x-
 * and y-axis.
 */
class AATriangle final
{
public:
  /** Directions:
   *
   *    SOUTHEWEST    NORTHEAST   SOUTHEAST    NORTHWEST
   *    *      or      *---*   or      *    or *---*
   *    | \             \  |         / |       |  /
   *    |  \             \ |        /  |       | /
   *    *---*              *       *---*       *
   *
   * Deform flags: (see docs/aatriangletypes.png for details)
   */
  enum Direction {
    SOUTHWEST = 0,
    NORTHEAST,
    SOUTHEAST,
    NORTHWEST,
    DIRECTION_MASK = 0x0003,
    DEFORM_BOTTOM = 0x0010,
    DEFORM_TOP = 0x0020,
    DEFORM_LEFT = 0x0030,
    DEFORM_RIGHT = 0x0040,
    DEFORM_MASK = 0x0070
  };

  static int vertical_flip(int dir);

public:
  AATriangle() :
    bbox(),
    dir(SOUTHWEST)
  {
  }
  AATriangle(const Rectf& newbbox, int newdir) :
    bbox(newbbox),
    dir(newdir)
  {
  }

  // Meant for checking directions
  explicit AATriangle(int newdir) :
    bbox(),
    dir(newdir)
  {
  }

  inline int get_dir() const { return dir; }
  inline bool is_south() const { return (dir & DIRECTION_MASK) == SOUTHWEST || (dir & DIRECTION_MASK) == SOUTHEAST; }
  inline bool is_east() const { return (dir & DIRECTION_MASK) == NORTHEAST || (dir & DIRECTION_MASK) == SOUTHEAST; }

public:
  Rectf bbox;
  int dir;
};

#endif

/* EOF */
