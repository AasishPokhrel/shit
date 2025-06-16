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

#include "badguy/fish_jumping.hpp"

#include "audio/sound_manager.hpp"
#include "object/explosion.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"

static const float FISH_JUMP_POWER = -600.f;
static const float FISH_WAIT_TIME = 1.f;
static const float FISH_BEACH_TIME = 5.f;

FishJumping::FishJumping(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/fish/forest/jumpfish.sprite"),
  m_wait_timer(),
  m_beached_timer(),
  m_stop_y(0)
{
  m_physic.enable_gravity(true);
  m_water_affected = false;
}

void
FishJumping::collision_solid(const CollisionHit& chit)
{
  hit(chit);
  if (!m_in_water && chit.bottom && !m_frozen)
  {
    m_physic.set_velocity_y(-300.f);
    if (!m_beached_timer.started())
      m_beached_timer.start(FISH_BEACH_TIME);
  }
  if (m_frozen)
    BadGuy::collision_solid(chit);
}

HitResponse
FishJumping::collision_badguy(BadGuy& , const CollisionHit& chit)
{
  if (m_beached_timer.started())
    collision_solid(chit);

  return hit(chit);
}

HitResponse
FishJumping::hit(const CollisionHit& hit_)
{
  if (hit_.top)
    m_physic.set_velocity_y(0);

  return CONTINUE;
}

void
FishJumping::collision_tile(uint32_t tile_attributes)
{
  if ((tile_attributes & Tile::WATER) && (m_physic.get_velocity_y() >= 0) && !m_frozen) {
    if (m_beached_timer.started())
      m_beached_timer.stop();
    // Initialize stop position if uninitialized.
    if (m_stop_y == 0) m_stop_y = get_pos().y + m_col.m_bbox.get_height();

    // Stop when we have reached the stop position.
    if (get_pos().y >= m_stop_y && m_physic.get_velocity_y() > 0.f) {
      if (!m_frozen)
        start_waiting();
      m_col.set_movement(Vector(0, 0));
      SoundManager::current()->play("sounds/splash.wav", get_pos());
    }
  }
  if ((!(tile_attributes & Tile::WATER) || m_frozen) && (tile_attributes & Tile::HURTS)) {
    kill_fall();
  }
}

void
FishJumping::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);
  m_in_water = !Sector::get().is_free_of_tiles(get_bbox(), true, Tile::WATER);

  if (m_beached_timer.check())
  {
    ignite();
    m_physic.reset();
    m_physic.enable_gravity(false);
    m_beached_timer.stop();
  }

  // Waited long enough?
  if (m_wait_timer.check())
    jump();

  // Set sprite.
  if (!m_frozen && !is_ignited())
    set_action((m_physic.get_velocity_y() == 0.f && m_in_water) ? "wait" :
      m_physic.get_velocity_y() < 0.f ? "default" : "down");

  // We can't afford flying out of the tilemap, 'cause the engine would remove us.
  if ((get_pos().y - 31.8f) < 0) // Too high, let us fall.
  {
    m_physic.set_velocity_y(0);
    m_physic.enable_gravity(true);
  }

  if (m_ignited && m_in_water)
    remove_me();
}

void
FishJumping::start_waiting()
{
  m_wait_timer.start(FISH_WAIT_TIME);
  m_physic.enable_gravity(false);
  m_physic.set_velocity_y(0);
}

void
FishJumping::jump()
{
  m_physic.set_velocity_y(FISH_JUMP_POWER);
  m_physic.enable_gravity(true);
}

void
FishJumping::freeze()
{
  BadGuy::freeze();
  m_physic.enable_gravity(true);
  set_action("iced");
  m_sprite->set_color(Color(1.0f, 1.0f, 1.0f));
  m_wait_timer.stop();
  if (m_beached_timer.started())
    m_beached_timer.stop();
}

void
FishJumping::unfreeze(bool melt)
{
  m_dir = Direction::LEFT;
  BadGuy::unfreeze(melt);
}

void
FishJumping::kill_fall()
{
  if (!is_ignited())
  set_action("default");
  BadGuy::kill_fall();
}

bool
FishJumping::is_freezable() const
{
  return true;
}

std::vector<Direction>
FishJumping::get_allowed_directions() const
{
  return {};
}

/* EOF */
