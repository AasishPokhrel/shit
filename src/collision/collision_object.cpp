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

#include "collision/collision_object.hpp"

#include "collision/collision_movement_manager.hpp"
#include "supertux/moving_object.hpp"

CollisionObject::CollisionObject(CollisionGroup group, MovingObject& parent) :
  m_parent(parent),
  m_bbox(),
  m_group(group),
  m_movement(0.0f, 0.0f),
  m_dest(),
  m_unisolid(false),
  m_pressure(),
  m_objects_hit_bottom(),
  m_ground_movement_manager(nullptr)
{
}

void
CollisionObject::collision_solid(const CollisionHit& hit)
{
  m_parent.collision_solid(hit);
}

bool
CollisionObject::collides(CollisionObject& other, const CollisionHit& hit) const
{
  return m_parent.collides(other.m_parent, hit);
}

HitResponse
CollisionObject::collision(CollisionObject& other, const CollisionHit& hit)
{
  return m_parent.collision(other.m_parent, hit);
}

void
CollisionObject::collision_tile(uint32_t tile_attributes)
{
  m_parent.collision_tile(tile_attributes);
}

void
CollisionObject::collision_moving_object_bottom(CollisionObject& other)
{
  if (m_group == COLGROUP_STATIC
    || m_group == COLGROUP_MOVING_STATIC)
  {
    m_objects_hit_bottom.insert(&other);
  }
}

void
CollisionObject::notify_object_removal(CollisionObject* other)
{
  m_objects_hit_bottom.erase(other);
}

void
CollisionObject::clear_bottom_collision_list()
{
  m_objects_hit_bottom.clear();
}

void
CollisionObject::propagate_movement(const Vector& movement)
{
  for (CollisionObject* other_object : m_objects_hit_bottom) {
    if (other_object->get_group() == COLGROUP_STATIC) continue;
    m_ground_movement_manager->register_movement(*this, *other_object, movement);
    other_object->propagate_movement(movement);
  }
}

bool
CollisionObject::is_valid() const
{
  return m_parent.is_valid();
}

/* EOF */
