//  SuperTux
//  Copyright (C) 2020 Maxim Bernard <mbernard2@videotron.ca>
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

#ifndef HEADER_SUPERTUX_COLLISION_COLLISION_MOVEMENT_MANAGER_HPP
#define HEADER_SUPERTUX_COLLISION_COLLISION_MOVEMENT_MANAGER_HPP

#include "collision/collision_object.hpp"
#include "object/tilemap.hpp"
#include "math/vector.hpp"

#include <unordered_map>

/**
 * This class takes care of moving objects that have collided on top of other moving
 * objects or on top of moving solid tiles.
 * 
 * This step is performed after the object updates and before the collision detection.
 */
class CollisionGroundMovementManager final
{
private:

  /** Utility class used internally. */
  class TargetMovementData final
  {
  public:
    TargetMovementData() :
      m_moving_objects(),
      m_moving_tilemaps()
    {}

    void register_movement(CollisionObject& moving_object, const Vector& movement);
    void register_movement(TileMap& moving_tilemap, const Vector& movement);

    const std::unordered_map<CollisionObject*, Vector>& get_objects_map() const
    {
      return m_moving_objects;
    }

    const std::unordered_map<TileMap*, Vector>& get_tilemaps_map() const
    {
      return m_moving_tilemaps;
    }
    
  private:
    std::unordered_map<CollisionObject*, Vector> m_moving_objects;
    std::unordered_map<TileMap*, Vector> m_moving_tilemaps;
  };

public:

  CollisionGroundMovementManager() :
    m_movements_per_target()
  {}

  void register_movement(CollisionObject& moving_object, CollisionObject& target_object, const Vector& movement);

  void register_movement(TileMap& moving_tilemap, CollisionObject& target_object, const Vector& movement);

  /** Moves all target objects according to their colliding object or tilemap whose movement
      vector has the lowest "y" coordinate.
      In other words: this makes sure objects always move up in case one of the colliding
      objects does. */
  void apply_all_ground_movement();


private:

  /** Holds all movement operations performed by objects or tilemaps which had moving
      objects that collided on top of them. */
  std::unordered_map<CollisionObject*, TargetMovementData> m_movements_per_target;


private:
  CollisionGroundMovementManager(const CollisionGroundMovementManager&) = delete;
  CollisionGroundMovementManager& operator=(const CollisionGroundMovementManager&) = delete;
};

#endif

/* EOF */
