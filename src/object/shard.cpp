//  SuperTux
//  Copyright (C) 2021 Daniel Ward <weluvgoatz@gmail.com>
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

#include "object/shard.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "math/util.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

constexpr float STICKING_TIME = 0.7f;
constexpr float FADEOUT_TIME = 0.3f;

Shard::Shard(const ReaderMapping& reader) :
  StickyObject(reader, "images/creatures/crystallo/shard.sprite", LAYER_TILES - 2, COLGROUP_MOVING),
  m_physic(),
  m_stick_timer(),
  m_fadeout_timer()
{
  m_physic.enable_gravity(true);
  SoundManager::current()->preload("sounds/crystallo-shardhit.ogg");
}

Shard::Shard(const Vector& pos, const Vector& velocity, const std::string& sprite) :
  StickyObject(pos, sprite, LAYER_TILES - 2, COLGROUP_MOVING),
  m_physic(),
  m_stick_timer(),
  m_fadeout_timer()
{
  m_physic.enable_gravity(true);
  m_physic.set_velocity(velocity);
  set_action("default");
  SoundManager::current()->preload("sounds/crystallo-shardhit.ogg");
}

void
Shard::update(float dt_sec)
{
  m_sticky = true;

  if (m_physic.get_velocity() != Vector(0.f, 0.f) && !m_sticking)
    m_sprite->set_angle(math::degrees(math::angle(Vector(m_physic.get_velocity_x(), m_physic.get_velocity_y()))));

  if (m_stick_timer.check())
    m_fadeout_timer.start(FADEOUT_TIME);

  if (m_fadeout_timer.check())
    remove_me();
  else if (m_fadeout_timer.started())
    m_sprite->set_alpha(1.0f - m_fadeout_timer.get_progress());

  m_col.set_movement(m_physic.get_movement(dt_sec));

  StickyObject::update(dt_sec);
}

void
Shard::collision_solid(const CollisionHit& hit)
{
  m_physic.set_velocity(0.f, 0.f);
  m_physic.set_acceleration(0.f, 0.f);
  m_physic.enable_gravity(hit.bottom);
  m_sticking = true;

  if (!m_stick_timer.started())
  {
    m_stick_timer.start(STICKING_TIME);
    SoundManager::current()->play("sounds/crystallo-shardhit.ogg", get_pos());
  }
}

HitResponse
Shard::collision(MovingObject& other, const CollisionHit&)
{
  // Do not hurt anyone while fading out
  if (m_fadeout_timer.started())
    return ABORT_MOVE;

  // ignore collisions with other shards
  auto shard = dynamic_cast<Shard*>(&other);
  if (&other == shard)
    return ABORT_MOVE;
  // kill badguys
  auto badguy = dynamic_cast<BadGuy*>(&other);
  if (badguy != nullptr)
    badguy->kill_fall();
  // kill players
  auto player = dynamic_cast<Player*>(&other);
  if (player != nullptr)
    player->kill(false);
  return ABORT_MOVE;
}

/* EOF */
