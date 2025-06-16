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

#include "badguy/stumpy.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"

static const float STUMPY_SPEED = 120;
static const float INVINCIBLE_TIME = 1;

Stumpy::Stumpy(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/mr_tree/stumpy.sprite","left","right", LAYER_OBJECTS,
                "images/objects/lightmap_light/lightmap_light-large.sprite"),
  mystate(STATE_NORMAL),
  invincible_timer()
{
  walk_speed = STUMPY_SPEED;
  set_ledge_behavior(LedgeBehavior::SMART);
  SoundManager::current()->preload("sounds/mr_treehit.ogg");
}

Stumpy::Stumpy(const Vector& pos, Direction d) :
  WalkingBadguy(pos, d, "images/creatures/mr_tree/stumpy.sprite","left","right"),
  mystate(STATE_INVINCIBLE),
  invincible_timer()
{
  walk_speed = STUMPY_SPEED;
  set_ledge_behavior(LedgeBehavior::SMART);
  SoundManager::current()->preload("sounds/mr_treehit.ogg");
  invincible_timer.start(INVINCIBLE_TIME);
}

void
Stumpy::initialize()
{
  switch (mystate) {
    case STATE_INVINCIBLE:
      set_action("dizzy", m_dir);
      m_col.m_bbox.set_size(m_sprite->get_current_hitbox_width(), m_sprite->get_current_hitbox_height());
      m_physic.set_velocity_x(0);
      break;
    case STATE_NORMAL:
      WalkingBadguy::initialize();
      break;
  }
}

void
Stumpy::active_update(float dt_sec)
{
  switch (mystate) {
    case STATE_INVINCIBLE:
      if (invincible_timer.check()) {
        mystate = STATE_NORMAL;
        WalkingBadguy::initialize();
      }
      BadGuy::active_update(dt_sec);
      break;
    case STATE_NORMAL:
      WalkingBadguy::active_update(dt_sec);
      break;
  }
}

bool
Stumpy::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return WalkingBadguy::collision_squished(object);

  // If we're still invincible, we ignore the hit.
  if (mystate == STATE_INVINCIBLE) {
    SoundManager::current()->play("sounds/mr_treehit.ogg", get_pos());
    auto player = dynamic_cast<Player*>(&object);
    if (player) player->bounce(*this);
    return true;
  }

  // If we can die, we do.
  if (mystate == STATE_NORMAL) {
    set_action("squished", m_dir);
    m_col.set_size(m_sprite->get_current_hitbox_width(), m_sprite->get_current_hitbox_height());
    kill_squished(object);
    // Spawn some particles.
    // TODO: Provide convenience function in MovingSprite or MovingObject?
    for (int i = 0; i < 25; i++) {
      Vector ppos = m_col.m_bbox.get_middle();
      float angle = graphicsRandom.randf(-math::PI_2, math::PI_2);
      float velocity = graphicsRandom.randf(45, 90);
      float vx = sinf(angle)*velocity;
      float vy = -cosf(angle)*velocity;
      Vector pspeed = Vector(vx, vy);
      Vector paccel = Vector(0, Sector::get().get_gravity()*10);
      Sector::get().add<SpriteParticle>("images/particles/bark.sprite",
                                             "default",
                                             ppos, ANCHOR_MIDDLE,
                                             pspeed, paccel,
                                             LAYER_OBJECTS-1);
    }

    return true;

  }

  // TODO: Handle exception or add relevant logic here.
  return true;
}

void
Stumpy::collision_solid(const CollisionHit& hit)
{
  update_on_ground_flag(hit);

  switch (mystate) {
    case STATE_INVINCIBLE:
      if (hit.top || hit.bottom) {
        m_physic.set_velocity_y(0);
      }
      if (hit.left || hit.right) {
        m_physic.set_velocity_x(0);
      }
      break;
    case STATE_NORMAL:
      WalkingBadguy::collision_solid(hit);
      break;
  }
}

HitResponse
Stumpy::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  switch (mystate) {
    case STATE_INVINCIBLE:
      if (hit.top || hit.bottom) {
        m_physic.set_velocity_y(0);
      }
      if (hit.left || hit.right) {
        m_physic.set_velocity_x(0);
      }
      return CONTINUE;

    case STATE_NORMAL:
      return WalkingBadguy::collision_badguy(badguy, hit);
  }
  return CONTINUE;
}

bool
Stumpy::is_freezable() const
{
  return true;
}

/* EOF */
