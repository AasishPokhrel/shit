//  SuperTux
//  Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
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

#include "badguy/kamikazesnowball.hpp"

#include "audio/sound_manager.hpp"

namespace{
  static const float KAMIKAZE_SPEED = 200;
  static const float LEAFSHOT_SPEED = 400;
  const std::string SPLAT_SOUND = "sounds/splat.wav";
}

KamikazeSnowball::KamikazeSnowball(const ReaderMapping& reader, const std::string& sprite_name) :
  BadGuy(reader, sprite_name)
{
  SoundManager::current()->preload(SPLAT_SOUND);
  set_action (m_dir, /* loops = */ -1);
}

void
KamikazeSnowball::initialize()
{
  m_physic.set_velocity_x(m_dir == Direction::LEFT ? -KAMIKAZE_SPEED : KAMIKAZE_SPEED);
  m_physic.enable_gravity(false);
  set_action(m_dir);
}

bool
KamikazeSnowball::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return BadGuy::collision_squished(object);
  set_action("squished", m_dir);
  kill_squished(object);
  return true;
}

void
KamikazeSnowball::collision_solid(const CollisionHit& hit)
{
  if (!m_frozen)
  {
    if (hit.top || hit.bottom) {
      m_physic.set_velocity_y(0);
    }
    if (hit.left || hit.right) {
      kill_collision();
    }
  }
  else
    BadGuy::collision_solid(hit);
}

void
KamikazeSnowball::kill_collision()
{
  set_action("collision", m_dir);
  SoundManager::current()->play(SPLAT_SOUND, get_pos());
  m_physic.set_velocity(0, 0);
  m_physic.enable_gravity(true);
  set_state(STATE_FALLING);

  run_dead_script();
}

HitResponse
KamikazeSnowball::collision_player(Player& player, const CollisionHit& hit)
{
  // Methodology to determine necessity of death.
  if (!m_frozen)
  {
    HitResponse response = BadGuy::collision_player(player, hit);
    if (response == FORCE_MOVE) {
      kill_collision();
    }
  }
  else
  {
    BadGuy::collision_player(player, hit);
    return FORCE_MOVE;
  }
  return ABORT_MOVE;
}

LeafShot::LeafShot(const ReaderMapping& reader) :
  KamikazeSnowball(reader, "images/creatures/leafshot/leafshot.sprite")
{
  parse_type(reader);
}

void
LeafShot::initialize()
{
  m_physic.set_velocity_x(m_dir == Direction::LEFT ? -LEAFSHOT_SPEED : LEAFSHOT_SPEED);
  m_physic.enable_gravity(false);
  set_action(m_dir);
}

GameObjectTypes
LeafShot::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "corrupted", _("Corrupted") }
  };
}

std::string
LeafShot::get_default_sprite_name() const
{
  switch (m_type)
  {
    case CORRUPTED:
      return "images/creatures/leafshot/corrupted/rotshot.sprite";
    default:
      return m_default_sprite_name;
  }
}

bool
LeafShot::is_freezable() const
{
  return true;
}

void
LeafShot::freeze()
{
  BadGuy::freeze();
  m_physic.enable_gravity(true);
}

void
LeafShot::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
  m_physic.enable_gravity(false);
  initialize();
}

bool
LeafShot::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return BadGuy::collision_squished(object);
  set_action("squished", m_dir);
  // Spawn death particles.
  spawn_explosion_sprites(3, "images/particles/leafshot.sprite");
  kill_squished(object);
  return true;
}

void
LeafShot::kill_collision()
{
  KamikazeSnowball::kill_collision();
  spawn_explosion_sprites(3, "images/particles/leafshot.sprite");
}

/* EOF */
