//  SuperTux - "Totem" Badguy
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "badguy/totem.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"

static const float JUMP_ON_SPEED_Y = -400;
static const float JUMP_OFF_SPEED_Y = -500;
static const std::string LAND_ON_TOTEM_SOUND = "sounds/totem.ogg";

Totem::Totem(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/totem/totem.sprite"),
  carrying(nullptr),
  carried_by(nullptr)
{
  SoundManager::current()->preload( LAND_ON_TOTEM_SOUND );
}

Totem::~Totem()
{
  if (carrying) carrying->jump_off();
  if (carried_by) jump_off();
}

bool
Totem::updatePointers(const GameObject* from_object, GameObject* to_object)
{
  if (from_object == carrying) {
    carrying = dynamic_cast<Totem*>(to_object);
    return true;
  }
  if (from_object == carried_by) {
    carried_by = dynamic_cast<Totem*>(to_object);
    return true;
  }
  return false;
}

void
Totem::initialize()
{
  if (!carried_by) {
static const float WALKSPEED = 100;
    m_physic.set_velocity_x(m_dir == Direction::LEFT ? -WALKSPEED : WALKSPEED);
    set_action("walking", m_dir);
    return;
  } else {
    synchronize_with(carried_by);
    set_action("stacked", m_dir);
    return;
  }
}

void
Totem::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);

  if (!carried_by) {
    if (on_ground() && might_fall())
    {
      m_dir = (m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT);
      initialize();
    }

    // Jump slightly if we encounter a suitable totem.
    for (auto& t : Sector::get().get_objects_by_type<Totem>()) {
      // Skip if we are not approaching each other.
      if (!((m_dir == Direction::LEFT) && (t.m_dir == Direction::RIGHT))) continue;

      Vector p1 = m_col.m_bbox.p1();
      Vector p2 = t.get_pos();

      // Skip if we are not on the same height.
      float dy = (p1.y - p2.y);
      if (fabsf(dy - 0) > 2) continue;

      // Skip if the totem is too far away.
      float dx = (p1.x - p2.x);
      if (fabsf(dx - 128) > 2) continue;

      m_physic.set_velocity_y(JUMP_ON_SPEED_Y);
      p1.y -= 1;
      set_pos(p1);
      break;
    }
  }

  if (carried_by) {
    synchronize_with(carried_by);
  }

  if (carrying) {
    carrying->synchronize_with(this);
  }

}

bool
Totem::collision_squished(MovingObject& object)
{
  /// Tux shouldn't be able to bisect totem stack by sacrificing his powerup.
  /// --Hume2
  if (carrying) {
    return false;
  }

  if (carried_by) {
    auto player = dynamic_cast<Player*>(&object);
    if (player) player->bounce(*this);
    jump_off();
  }

  set_action("squished", m_dir);
  m_col.m_bbox.set_size(m_sprite->get_current_hitbox_width(), m_sprite->get_current_hitbox_height());

  kill_squished(object);
  return true;
}

void
Totem::collision_solid(const CollisionHit& hit)
{
  update_on_ground_flag(hit);

  // If we are being carried around: pass event to bottom of stack and ignore it.
  if (carried_by) {
    carried_by->collision_solid(hit);
    return;
  }

  // If we hit something from above or below: stop moving in this direction.
  if (hit.top || hit.bottom) {
    m_physic.set_velocity_y(0);
  }

  // If we are hit from the direction we are facing: turn around.
  if (hit.left && (m_dir == Direction::LEFT)) {
    m_dir = Direction::RIGHT;
    initialize();
  }
  if (hit.right && (m_dir == Direction::RIGHT)) {
    m_dir = Direction::LEFT;
    initialize();
  }
}

HitResponse
Totem::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  // If we are being carried around: pass event to bottom of stack and ignore it.
  if (carried_by) {
    carried_by->collision_badguy(badguy, hit);
    return CONTINUE;
  }

  // If we hit a Totem that is not from our stack: have our base jump on its top.
  auto totem = dynamic_cast<Totem*>(&badguy);
  if (totem) {
    auto thisBase = this; while (thisBase->carried_by) thisBase=thisBase->carried_by;
    auto srcBase = totem; while (srcBase->carried_by)  srcBase=srcBase->carried_by;
    auto thisTop = this;  while (thisTop->carrying)    thisTop=thisTop->carrying;
    if (srcBase != thisBase) {
      srcBase->jump_on(thisTop);
    }
  }

  // If we are hit from the direction we are facing: turn around.
  if (hit.left && (m_dir == Direction::LEFT)) {
    m_dir = Direction::RIGHT;
    initialize();
  }
  if (hit.right && (m_dir == Direction::RIGHT)) {
    m_dir = Direction::LEFT;
    initialize();
  }

  return CONTINUE;
}

void
Totem::kill_fall()
{
  if (carrying) carrying->jump_off();
  if (carried_by) jump_off();

  BadGuy::kill_fall();
}

void
Totem::jump_on(Totem* target)
{
  if (target->carrying) {
    log_warning << "Target is already carrying someone." << std::endl;
    return;
  }

  target->carrying = this;

  carried_by = target;
  initialize();
  m_col.m_bbox.set_size(m_sprite->get_current_hitbox_width(), m_sprite->get_current_hitbox_height());

  SoundManager::current()->play( LAND_ON_TOTEM_SOUND , get_pos());

  synchronize_with(target);
}

void
Totem::jump_off() {
  if (!carried_by) {
    log_warning << "Not carried by anyone." << std::endl;
    return;
  }

  carried_by->carrying = nullptr;

  carried_by = nullptr;

  initialize();
  m_col.m_bbox.set_size(m_sprite->get_current_hitbox_width(), m_sprite->get_current_hitbox_height());

  m_physic.set_velocity_y(JUMP_OFF_SPEED_Y);
}

void
Totem::synchronize_with(Totem* base)
{

  if (m_dir != base->m_dir) {
    m_dir = base->m_dir;
    set_action("stacked", m_dir);
  }

  Vector pos = base->get_pos();
  pos.y -= m_sprite->get_current_hitbox_height();
  set_pos(pos);

  m_physic.set_velocity(base->m_physic.get_velocity());
}

/* EOF */
