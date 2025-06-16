//  SuperTux
//  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#include "badguy/snowman.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/snowball.hpp"
#include "object/bullet.hpp"
#include "object/player.hpp"
#include "supertux/sector.hpp"

Snowman::Snowman(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/snowman/snowman.sprite", "left", "right")
{
  walk_speed = 40;
  SoundManager::current()->preload("sounds/pop.ogg");
}

void
Snowman::loose_head()
{
  // Replace with Snowball.
  Vector snowball_pos = get_pos();
  // Hard-coded values from sprites.
  snowball_pos.x += 5;
  snowball_pos.y += 1;

  /* Create a death animation for the (now headless) snowman. */
  set_action ("headless", m_dir, /* loops = */ -1);
  set_pos (get_pos () + Vector (-4.0, 19.0)); /* Difference in the sprite offsets. */
  m_physic.set_velocity_y(0);
  m_physic.set_acceleration_y(0);
  m_physic.enable_gravity(true);
  set_state (STATE_FALLING);
  m_countMe = false;

  /* Create a new snowball where the snowman's head was. */
  Sector::get().add<SnowBall>(snowball_pos, m_dir, m_dead_script);
}

HitResponse
Snowman::collision_bullet(Bullet& bullet, const CollisionHit& hit)
{
  if (bullet.get_type() == BONUS_FIRE) {
    // Fire bullets destroy snowman's body.
    Vector snowball_pos = get_pos();
    // Hard-coded values from sprites.
    snowball_pos.x += 5;
    snowball_pos.y += 1;

    /* Create a new snowball where the snowman's head was. */
    Sector::get().add<SnowBall>(snowball_pos, m_dir, m_dead_script);
    m_countMe = false;

    SoundManager::current()->play("sounds/pop.ogg", get_pos()); // This could be a different sound.
    bullet.remove_me();
    ignite();

    return ABORT_MOVE;
  }
  else {
    // In all other cases, bullets ricochet.
    bullet.ricochet(*this, hit);
    return FORCE_MOVE;
  }
}

bool
Snowman::collision_squished(MovingObject& object)
{
  auto player = dynamic_cast<Player*>(&object);
  if (player && (player->m_does_buttjump || player->is_invincible())) {
    player->bounce(*this);
    kill_fall();
    return true;
  }

  if (player)
    player->bounce(*this);

  SoundManager::current()->play("sounds/pop.ogg", get_pos());

  loose_head();

  return true;
}

/* EOF */
