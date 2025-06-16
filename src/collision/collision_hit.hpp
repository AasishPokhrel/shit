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

#ifndef HEADER_SUPERTUX_COLLISION_COLLISION_HIT_HPP
#define HEADER_SUPERTUX_COLLISION_COLLISION_HIT_HPP

#include "math/vector.hpp"

/**
 * Used as return value for the collision functions, to indicate how the
 * collision should be handled
 */
enum HitResponse
{
  // Dynamic collision responses

  /// Call collision() but do no collision handling
  ABORT_MOVE = 0,
  /// move object out of collision and check for collisions again
  /// if this happens too often then the move will just be aborted
  /// (normal physics)
  CONTINUE,
  /// Treat object as kinematic, with infinite inertia/mass
  /// pushing other (CONTINUE) objects out of the way
  FORCE_MOVE
};

/**
 * This class collects data about a collision
 */
class CollisionHit final
{
public:
  CollisionHit() :
    left(false),
    right(false),
    top(false),
    bottom(false),
    crush(false),
    slope_normal(0.0f, 0.0f)
  {}

  bool left, right;
  bool top, bottom;
  bool crush;

  Vector slope_normal;
};

#endif

/* EOF */
