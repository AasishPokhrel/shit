//  SuperTux - WalkingBadguy
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

#include "badguy/walking_badguy.hpp"

#include <math.h>

#include "sprite/sprite.hpp"

WalkingBadguy::WalkingBadguy(const Vector& pos,
                             const std::string& sprite_name_,
                             const std::string& walk_left_action_,
                             const std::string& walk_right_action_,
                             int layer_,
                             const std::string& light_sprite_name) :
  BadGuy(pos, sprite_name_, layer_, light_sprite_name),
  walk_left_action(walk_left_action_),
  walk_right_action(walk_right_action_),
  walk_speed(80),
  max_drop_height(-1),
  turn_around_timer(),
  turn_around_counter(),
  m_stay_on_platform_overridden(false)
{
}

WalkingBadguy::WalkingBadguy(const Vector& pos,
                             Direction direction,
                             const std::string& sprite_name_,
                             const std::string& walk_left_action_,
                             const std::string& walk_right_action_,
                             int layer_,
                             const std::string& light_sprite_name) :
  BadGuy(pos, direction, sprite_name_, layer_, light_sprite_name),
  walk_left_action(walk_left_action_),
  walk_right_action(walk_right_action_),
  walk_speed(80),
  max_drop_height(-1),
  turn_around_timer(),
  turn_around_counter(),
  m_stay_on_platform_overridden(false)
{
}

WalkingBadguy::WalkingBadguy(const ReaderMapping& reader,
                             const std::string& sprite_name_,
                             const std::string& walk_left_action_,
                             const std::string& walk_right_action_,
                             int layer_,
                             const std::string& light_sprite_name) :
  BadGuy(reader, sprite_name_, layer_, light_sprite_name),
  walk_left_action(walk_left_action_),
  walk_right_action(walk_right_action_),
  walk_speed(80),
  max_drop_height(-1),
  turn_around_timer(),
  turn_around_counter(),
  m_stay_on_platform_overridden(false)
{
}

void
WalkingBadguy::initialize()
{
  if (m_frozen)
    return;
  set_action(m_dir == Direction::LEFT ? walk_left_action : walk_right_action);
  m_col.m_bbox.set_size(m_sprite->get_current_hitbox_width(), m_sprite->get_current_hitbox_height());
  m_physic.set_velocity_x(m_dir == Direction::LEFT ? -walk_speed : walk_speed);
  m_physic.set_acceleration_x (0.0);
}

void
WalkingBadguy::set_walk_speed (float ws)
{
  walk_speed = fabsf(ws);
  /* physic.set_velocity_x(dir == LEFT ? -walk_speed : walk_speed); */
}

void WalkingBadguy::set_ledge_behavior(LedgeBehavior behavior)
{
  switch (behavior)
  {
    case LedgeBehavior::STRICT:
      max_drop_height = 0;
      break;

    case LedgeBehavior::SMART:
      max_drop_height = 16.f;
      break;

    case LedgeBehavior::NORMAL:
      max_drop_height = s_normal_max_drop_height;
      break;

    case LedgeBehavior::FALL:
      max_drop_height = -1;
      break;
  }
}

void
WalkingBadguy::add_velocity (const Vector& velocity)
{
  m_physic.set_velocity(m_physic.get_velocity() + velocity);
}

void
WalkingBadguy::active_update(float dt_sec, float dest_x_velocity, float modifier)
{
  BadGuy::active_update(dt_sec);

  // Walk down slopes smoothly.
  if (on_ground() && m_floor_normal.y != 0 && (m_floor_normal.x * m_physic.get_velocity_x()) >= 0) {
    m_physic.set_velocity_y((std::abs(m_physic.get_velocity_x()) * std::abs(m_floor_normal.x)) + 100.f);
  }

  float current_x_velocity = m_physic.get_velocity_x ();

  if (m_frozen)
    return;
  /* We're very close to our target speed. Just set it to avoid oscillation */
  if ((current_x_velocity > (dest_x_velocity - 5.0f)) &&
           (current_x_velocity < (dest_x_velocity + 5.0f)))
  {
    m_physic.set_velocity_x (dest_x_velocity);
    m_physic.set_acceleration_x (0.0);
  }
  /* Check if we're going too slow or even in the wrong direction */
  else if (((dest_x_velocity <= 0.0f) && (current_x_velocity > dest_x_velocity)) ||
           ((dest_x_velocity > 0.0f) && (current_x_velocity < dest_x_velocity)))
  {
    /* acceleration == walk-speed => it will take one second to get from zero
     * to full speed. */
    m_physic.set_acceleration_x (dest_x_velocity * modifier);
  }
  /* Check if we're going too fast */
  else if (((dest_x_velocity <= 0.0f) && (current_x_velocity < dest_x_velocity)) ||
           ((dest_x_velocity > 0.0f) && (current_x_velocity > dest_x_velocity)))
  {
    /* acceleration == walk-speed => it will take one second to get twice the
     * speed to normal speed. */
    m_physic.set_acceleration_x ((-1.f) * dest_x_velocity);
  }
  else
  {
    /* The above should have covered all cases. */
    assert(false);
  }

  if (max_drop_height > -1 && on_ground() && might_fall(max_drop_height+1) && !m_stay_on_platform_overridden)
    turn_around();
  m_stay_on_platform_overridden = false;

  if ((m_dir == Direction::LEFT) && (m_physic.get_velocity_x () > 0.0f)) {
    m_dir = Direction::RIGHT;
    set_action (walk_right_action, /* loops = */ -1);
  }
  else if ((m_dir == Direction::RIGHT) && (m_physic.get_velocity_x () < 0.0f)) {
    m_dir = Direction::LEFT;
    set_action (walk_left_action, /* loops = */ -1);
  }
}

void
WalkingBadguy::active_update(float dt_sec)
{
  active_update (dt_sec, (m_dir == Direction::LEFT) ? -walk_speed : +walk_speed);
}

void
WalkingBadguy::collision_solid(const CollisionHit& hit)
{

  update_on_ground_flag(hit);

  if (m_frozen)
  {
    BadGuy::collision_solid(hit);
    return;
  }

  if (hit.top) {
    if (m_physic.get_velocity_y() < 0) m_physic.set_velocity_y(0);
  }
  if (hit.bottom) {
    if (m_physic.get_velocity_y() > 0) m_physic.set_velocity_y(0);
  }

  if ( hit.slope_normal.x == 0.0f &&
      ((hit.left && m_dir == Direction::LEFT) ||
      (hit.right && m_dir == Direction::RIGHT)) ) {
      turn_around();
  }

}

HitResponse
WalkingBadguy::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (hit.top) {
    return FORCE_MOVE;
  }

  if (badguy.is_frozen())
    collision_solid(hit);

  if ((hit.left && (m_dir == Direction::LEFT)) || (hit.right && (m_dir == Direction::RIGHT))) {
    turn_around();
  }

  return CONTINUE;
}

void
WalkingBadguy::turn_around()
{
  if (m_frozen)
    return;
  m_dir = m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
  if (get_state() == STATE_INIT || get_state() == STATE_INACTIVE || get_state() == STATE_ACTIVE) {
    set_action(m_dir == Direction::LEFT ? walk_left_action : walk_right_action);
  }
  m_physic.set_velocity_x(-m_physic.get_velocity_x());
  m_physic.set_acceleration_x (-m_physic.get_acceleration_x ());

  // if we get dizzy, we fall off the screen
  if (turn_around_timer.started()) {
    if (turn_around_counter++ > 10) kill_fall();
  } else {
    turn_around_timer.start(1);
    turn_around_counter = 0;
  }

}

void
WalkingBadguy::freeze()
{
  BadGuy::freeze();
}

void
WalkingBadguy::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
  WalkingBadguy::initialize();
}

/* EOF */
