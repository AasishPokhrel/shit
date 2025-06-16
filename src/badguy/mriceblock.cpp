//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023 Vankata453
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

#include "badguy/mriceblock.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "object/bullet.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "sprite/sprite.hpp"
#include "supertux/constants.hpp"
#include "supertux/sector.hpp"

namespace {
  const float KICKSPEED = 500;
  const int MAXSQUISHES = 10;
  const float NOKICK_TIME = 0.1f;
}

MrIceBlock::MrIceBlock(const ReaderMapping& reader, const std::string& sprite_name) :
  WalkingBadguy(reader, sprite_name, "left", "right"),
  ice_state(ICESTATE_NORMAL),
  nokick_timer(),
  flat_timer(),
  squishcount(0)
{
  parse_type(reader);

  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::FALL);
  SoundManager::current()->preload("sounds/iceblock_bump.wav");
  SoundManager::current()->preload("sounds/stomp.wav");
  SoundManager::current()->preload("sounds/kick.wav");
}

void
MrIceBlock::initialize()
{
  WalkingBadguy::initialize();
  set_state(ICESTATE_NORMAL);
}

GameObjectTypes
MrIceBlock::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "laptop", _("Laptop") }
  };
}

std::string
MrIceBlock::get_default_sprite_name() const
{
  switch (m_type)
  {
    case LAPTOP:
      return "images/creatures/laptop/laptop.sprite";
    default:
      return m_default_sprite_name;
  }
}

bool
MrIceBlock::is_freezable() const
{
  return m_type == LAPTOP;
}

void
MrIceBlock::freeze()
{
  WalkingBadguy::freeze();

  if (ice_state == ICESTATE_KICKED)
    set_state(ICESTATE_FLAT);
}

void
MrIceBlock::unfreeze(bool melt)
{
  WalkingBadguy::unfreeze(melt);

  // Wake up on unfreeze, if flat.
  if (ice_state == ICESTATE_FLAT)
    set_state(ICESTATE_WAKING);
}

void
MrIceBlock::active_update(float dt_sec)
{
  if (m_frozen)
  {
    if (ice_state == ICESTATE_FLAT)
    {
      const float& vel_x = m_physic.get_velocity_x();

      // Gradually slow down, when frozen in FLAT state, if any horizontal velocity is present.
      if ((m_dir == Direction::LEFT && vel_x < 0.f) || (m_dir == Direction::RIGHT && vel_x > 0.f))
        m_physic.set_velocity_x(vel_x * 0.95f);
      else
        m_physic.set_velocity_x(0.f);
    }

    BadGuy::active_update(dt_sec);
    return;
  }

  if (ice_state == ICESTATE_GRABBED || is_grabbed())
    return;

  if (ice_state == ICESTATE_FLAT && flat_timer.check()) {
    set_state(ICESTATE_WAKING);
  }

  if (ice_state == ICESTATE_WAKING && m_sprite->animation_done()) {
    set_state(ICESTATE_NORMAL);
  }

  if (ice_state == ICESTATE_NORMAL)
  {
    WalkingBadguy::active_update(dt_sec);
    return;
  }

  BadGuy::active_update(dt_sec);
}

bool
MrIceBlock::can_break() const {
  return ice_state == ICESTATE_KICKED || ice_state == ICESTATE_FLAT;
}

void
MrIceBlock::collision_solid(const CollisionHit& hit)
{
  update_on_ground_flag(hit);

  if (hit.top || hit.bottom) { // Floor or roof.
    m_physic.set_velocity_y(0);
  }

  // Hit left or right.
  switch (ice_state) {
  case ICESTATE_NORMAL:
    WalkingBadguy::collision_solid(hit);
    break;
  case ICESTATE_KICKED: {
    if (hit.left || hit.right) {
      if ((hit.right && m_dir == Direction::RIGHT) || (hit.left && m_dir == Direction::LEFT)) {
        m_dir = (m_dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT;
      }
      SoundManager::current()->play("sounds/iceblock_bump.wav", get_pos());
      m_physic.set_velocity_x(-m_physic.get_velocity_x() * .975f);
    }
    set_action("flat", m_dir, /* loops = */ -1);
    if (fabsf(m_physic.get_velocity_x()) < walk_speed * 1.5f)
      set_state(ICESTATE_NORMAL);
    break;
  }
  case ICESTATE_FLAT:
    if (m_frozen)
    {
      if ((hit.right && m_dir == Direction::RIGHT) || (hit.left && m_dir == Direction::LEFT))
      {
        m_dir = (m_dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT;
        m_physic.inverse_velocity_x();
      }
      break;
    }
    [[fallthrough]];
  case ICESTATE_WAKING:
    m_physic.set_velocity_x(0);
    break;
  case ICESTATE_GRABBED:
    break;
  }
}

HitResponse
MrIceBlock::collision(MovingObject& object, const CollisionHit& hit)
{
  if (m_frozen)
  {
    auto bullet = dynamic_cast<Bullet*>(&object);
    if (bullet)
      return BadGuy::collision_bullet(*bullet, hit);

    return FORCE_MOVE;
  }

  if (ice_state == ICESTATE_GRABBED)
    return FORCE_MOVE;

  return BadGuy::collision(object, hit);
}

HitResponse
MrIceBlock::collision_player(Player& player, const CollisionHit& hit)
{
  // Handle kicks from left or right side.
  if ((ice_state == ICESTATE_WAKING || ice_state == ICESTATE_FLAT) && get_state() == STATE_ACTIVE) {
    if (hit.left) {
      m_dir = Direction::RIGHT;
      player.kick();
      set_state(ICESTATE_KICKED);
      return FORCE_MOVE;
    }
    else if (hit.right) {
      m_dir = Direction::LEFT;
      player.kick();
      set_state(ICESTATE_KICKED);
      return FORCE_MOVE;
    }
  }

  return BadGuy::collision_player(player, hit);
}

HitResponse
MrIceBlock::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  switch (ice_state) {
  case ICESTATE_NORMAL:
    return WalkingBadguy::collision_badguy(badguy, hit);
  case ICESTATE_FLAT:
  case ICESTATE_WAKING:
    return FORCE_MOVE;
  case ICESTATE_KICKED:
    badguy.kill_fall();
    return FORCE_MOVE;
  default:
    assert(false);
  }

  return ABORT_MOVE;
}

bool
MrIceBlock::collision_squished(MovingObject& object)
{
  Player* player = dynamic_cast<Player*>(&object);
  if (player && (player->m_does_buttjump || player->is_invincible())) {
    player->bounce(*this);
    kill_fall();
    return true;
  }

  switch (ice_state)
  {
  case ICESTATE_KICKED:
  {
    auto badguy = dynamic_cast<BadGuy*>(&object);
    if (badguy) {
      badguy->kill_fall();
      break;
    }
  }
  [[fallthrough]];

  case ICESTATE_NORMAL:
  {
    squishcount++;
    if (squishcount >= MAXSQUISHES) {
      kill_fall();
      return true;
    }
  }

  SoundManager::current()->play("sounds/stomp.wav", get_pos());
  m_physic.set_velocity(0, 0);
  set_state(ICESTATE_FLAT);
  nokick_timer.start(NOKICK_TIME);
  break;

  case ICESTATE_FLAT:
  case ICESTATE_WAKING:
  {
    if (object.get_pos().x < get_pos().x) {
      m_dir = Direction::RIGHT;
    }
    else {
      m_dir = Direction::LEFT;
    }
  }
  if (nokick_timer.check()) set_state(ICESTATE_KICKED);
  break;

  case ICESTATE_GRABBED:
    assert(false);
    break;
  }

  if (player) player->bounce(*this);
  return true;
}

void
MrIceBlock::set_state(IceState state_)
{
  if (ice_state == state_)
    return;

  switch (state_) {
  case ICESTATE_NORMAL:
    set_action(m_dir, /* loops = */ -1);
    WalkingBadguy::initialize();
    break;
  case ICESTATE_FLAT:
    set_action("flat", m_dir, /* loops = */ -1);
    flat_timer.start(4);
    break;
  case ICESTATE_KICKED:
    SoundManager::current()->play("sounds/kick.wav", get_pos());

    m_physic.set_velocity_x(m_dir == Direction::LEFT ? -KICKSPEED : KICKSPEED);
    set_action("flat", m_dir, /* loops = */ -1);
    // We should slide above 1 block holes now.
    m_col.m_bbox.set_size(34, 31.8f);
    break;
  case ICESTATE_GRABBED:
    flat_timer.stop();
    break;
  case ICESTATE_WAKING:
    flat_timer.stop();
    set_action("waking", m_dir, /* loops = */ 1);
    break;
  default:
    assert(false);
  }
  ice_state = state_;
}

void
MrIceBlock::grab(MovingObject& object, const Vector& pos, Direction dir_)
{
  if (m_frozen)
  {
    BadGuy::grab(object, pos, dir_);
    return;
  }

  Portable::grab(object, pos, dir_);
  m_col.set_movement(pos - get_pos());
  m_physic.set_velocity(m_col.get_movement() * LOGICAL_FPS);
  m_dir = dir_;
  set_action("flat", m_dir, /* loops = */ -1);
  set_state(ICESTATE_GRABBED);
  set_colgroup_active(COLGROUP_DISABLED);
}

void
MrIceBlock::ungrab(MovingObject& object, Direction dir_)
{
  if (m_frozen)
  {
    BadGuy::ungrab(object, dir_);
    return;
  }

  auto player = dynamic_cast<Player*> (&object);
  if (player && (player->is_swimming() || player->is_water_jumping()))
  {
    // Move the ice cube slightly away to avoid instantly killing Tux.
    float swimangle = player->get_swimming_angle();
    m_col.m_bbox.move(Vector(std::cos(swimangle) * 48.f, std::sin(swimangle) * 48.f));
  }
  if (dir_ == Direction::UP) {
    m_physic.set_velocity_y(-KICKSPEED);
    if (std::abs(player->get_physic().get_velocity_x()) < 1.0f) {
      set_state(ICESTATE_FLAT);
    }
    else
    {
      m_dir = (m_physic.get_velocity_x() > 0) ? Direction::RIGHT : Direction::LEFT;
      set_state(ICESTATE_KICKED);
    }
  }
  else if (dir_ == Direction::DOWN) {
    Vector mov(0, 32);
    if (Sector::get().is_free_of_statics(get_bbox().moved(mov), this)) {
      // There is free space, so throw it down.
      SoundManager::current()->play("sounds/kick.wav", get_pos());
      m_physic.set_velocity_y(KICKSPEED);
    }
    set_state(ICESTATE_FLAT);
  }
  else {
    m_dir = dir_;
    set_state(ICESTATE_KICKED);
  }
  set_colgroup_active(COLGROUP_MOVING);
  Portable::ungrab(object, dir_);
}

bool
MrIceBlock::is_portable() const
{
  return (m_frozen || ice_state == ICESTATE_FLAT);
}

void
MrIceBlock::ignite() {
  set_state(ICESTATE_NORMAL);
  BadGuy::ignite();
}

/* EOF */
