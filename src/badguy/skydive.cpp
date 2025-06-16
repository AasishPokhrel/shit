//  SuperTux
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

#include "badguy/skydive.hpp"

#include "audio/sound_manager.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "sprite/sprite.hpp"
#include "supertux/constants.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"

SkyDive::SkyDive(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/skydive/skydive.sprite")
{
  SoundManager::current()->preload("sounds/explosion.wav");
  set_action("default");
}

void
SkyDive::collision_solid(const CollisionHit& hit)
{
  if (m_frozen)
  {
    BadGuy::collision_solid(hit);
    return;
  }
  if (hit.bottom) {
    explode ();
    return;
  }

  if (hit.left || hit.right)
    m_physic.set_velocity_x(0.0);
  explode();
  return;
}

HitResponse
SkyDive::collision_badguy(BadGuy&, const CollisionHit& hit)
{
  if (hit.bottom) {
    explode();
    return ABORT_MOVE;
  }

  return FORCE_MOVE;
}

void
SkyDive::grab(MovingObject& object, const Vector& pos, Direction dir_)
{
  Vector movement = pos - get_pos();
  m_col.set_movement(movement);
  m_dir = dir_;

  if (!m_frozen)
  {
    m_physic.set_velocity(movement.x * LOGICAL_FPS, 0.0);
    m_physic.set_acceleration_y(0.0);
  }
  m_physic.enable_gravity(false);
  set_group(COLGROUP_DISABLED);
  BadGuy::grab(object, pos, dir_);
}

void
SkyDive::ungrab(MovingObject& object, Direction dir_)
{
  auto player = dynamic_cast<Player*> (&object);
  // Handle swimming state of the player.
  if (player)
  {
    if (player->is_swimming() || player->is_water_jumping())
    {
      float swimangle = player->get_swimming_angle();
      m_physic.set_velocity(Vector(std::cos(swimangle) * 40.f, std::sin(swimangle) * 40.f) +
        player->get_physic().get_velocity());
    }
    // Handle non-swimming.
    else
    {
      // Handle x-movement based on the player's direction and velocity.
      if (fabsf(player->get_physic().get_velocity_x()) < 1.0f)
        m_physic.set_velocity_x(0.f);
      else if ((player->m_dir == Direction::LEFT && player->get_physic().get_velocity_x() <= -1.0f)
        || (player->m_dir == Direction::RIGHT && player->get_physic().get_velocity_x() >= 1.0f))
        m_physic.set_velocity_x(player->get_physic().get_velocity_x()
          + (player->m_dir == Direction::LEFT ? -10.f : 10.f));
      else
        m_physic.set_velocity_x(player->get_physic().get_velocity_x()
          + (player->m_dir == Direction::LEFT ? -330.f : 330.f));
      // Handle y-movement based on the player's direction and velocity.
      m_physic.set_velocity_y(dir_ == Direction::UP ? -500.f :
        dir_ == Direction::DOWN ? 500.f :
        player->get_physic().get_velocity_x() != 0.f ? -200.f : 0.f);
    }
  }
  else if (!m_frozen)
  {
    set_action("falling");
    m_physic.set_velocity_y(0);
    m_physic.set_acceleration_y(0);
  }
  m_physic.enable_gravity(true);
  set_group(m_frozen ? COLGROUP_MOVING_STATIC : COLGROUP_MOVING);
  BadGuy::ungrab(object, dir_);
}

bool
SkyDive::is_freezable() const
{
  return true;
}

HitResponse
SkyDive::collision_player(Player&, const CollisionHit& hit)
{
  if (hit.bottom) {
    explode();
    return ABORT_MOVE;
  }

  return FORCE_MOVE;
}

bool
SkyDive::collision_squished(MovingObject& obj)
{
  if (m_frozen)
    return BadGuy::collision_squished(obj);

  auto player = dynamic_cast<Player *>(&obj);
  if (player) {
    player->bounce(*this);
    return false;
  }

  explode();
  return false;
}

void
SkyDive::collision_tile(uint32_t tile_attributes)
{
  if (tile_attributes & Tile::HURTS)
  {
    explode();
  }
}

void
SkyDive::initialize()
{
  if (!m_owner)
  {
    set_action("falling");
    m_physic.set_velocity_y(0);
    m_physic.set_acceleration_y(0);
  }
  BadGuy::initialize();
}

void
SkyDive::kill_fall()
{
  explode();
}

void
SkyDive::explode()
{
  if (!is_valid())
    return;
  if (m_frozen)
    BadGuy::kill_fall();
  else
  {
    Sector::get().add<Explosion>(
      get_anchor_pos(m_col.m_bbox, ANCHOR_BOTTOM), EXPLOSION_STRENGTH_DEFAULT);

    remove_me();
  }
}

bool
SkyDive::is_portable() const
{
  return true;
}

std::vector<Direction>
SkyDive::get_allowed_directions() const
{
  return {};
}

/* EOF */
