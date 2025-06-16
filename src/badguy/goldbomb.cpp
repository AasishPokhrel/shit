//  SuperTux BadGuy GoldBomb - a bomb that throws up coins when exploding
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2013 LMH <lmh.0013@gmail.com>
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

#include "badguy/goldbomb.hpp"

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "badguy/haywire.hpp"
#include "badguy/owl.hpp"
#include "object/coin_explode.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

static const float HOP_HEIGHT = -250.f;
static const float REALIZE_TIME = 0.5f;

// SAFE_DIST >= REALIZE_DIST
static const float REALIZE_DIST = 32.f * 8.f;
static const float SAFE_DIST = 32.f * 10.f;

static const float NORMAL_WALK_SPEED = 80.0f;
static const float FLEEING_WALK_SPEED = 180.0f;

GoldBomb::GoldBomb(const ReaderMapping& reader) :
  MrBomb(reader, "images/creatures/gold_bomb/gold_bomb.sprite"),
  m_realize_timer()
{
  assert(SAFE_DIST >= REALIZE_DIST);
}

void
GoldBomb::collision_solid(const CollisionHit& hit)
{
  // The first condition checks if the current state
  // is not walking nor ticking, which are both MrBomb states.
  if (m_state >= MB_STATE_COUNT && (hit.left || hit.right))
  {
    cornered();
    return;
  }

  MrBomb::collision_solid(hit);
}

void
GoldBomb::active_update(float dt_sec)
{
  if (m_state == MB_STATE_TICKING)
  {
    update_ticking(dt_sec);
    return;
  }

  if ((m_state == GB_STATE_FLEEING || m_state == GB_STATE_CORNERED || m_state == GB_STATE_RECOVER)
      && on_ground() && might_fall(s_normal_max_drop_height+1))
  {
    // If it's fleeing, enter cornered state once ledge is detected.
    // Skip the WalkingBadguy update function to prevent turning around
    // when a ledge is detected.
    if (m_state != GB_STATE_RECOVER)
      cornered();

    return;
  }

  WalkingBadguy::active_update(dt_sec);

  if (m_frozen) return;

  // Look for any of these in safe distance:
  // Player, ticking Haywire, ticking Bomb or ticking GoldBomb
  MovingObject* obj = nullptr;
  std::vector<MovingObject*> objs = Sector::get().get_nearby_objects(get_bbox().get_middle(), SAFE_DIST);
  for (MovingObject* currobj : objs)
  {
    obj = currobj;

    auto player = dynamic_cast<Player*>(obj);
    if (player && !player->get_ghost_mode())
      break;

    auto haywire = dynamic_cast<Haywire*>(obj);
    if (haywire && haywire->is_exploding())
      break;

    auto bomb = dynamic_cast<MrBomb*>(obj);
    if (bomb && bomb->is_ticking())
      break;

    obj = nullptr;
  }

  if (!obj)
  {
    // Everybody's outside of safe distance. Am I cornered?

    if (m_state == GB_STATE_CORNERED)
    {
      // Look back to check.
      recover();
      return;
    }

    if (m_state == GB_STATE_RECOVER)
    {
      if (!m_sprite->animation_done())
        return;
    }

    normalize();
    return;
  }

  // Someone's in safe distance
  const Vector p1      = get_bbox().get_middle();
  const Vector p2      = obj->get_bbox().get_middle();
  const Vector vecdist = p2-p1;

  // But I only react to those who are in realize distance
  if (glm::length(vecdist) > REALIZE_DIST && m_state == MB_STATE_NORMAL)
    return;

  // Someone's around!
  switch (m_state)
  {
    case GB_STATE_FLEEING:
      // They popped up from the other side! Turn around!
      if (m_dir == (vecdist.x > 0 ? Direction::LEFT : Direction::RIGHT)) return;
      [[fallthrough]];

    case MB_STATE_NORMAL:
    {
      if (!on_ground()) break;

      // Gold bomb is solid therefore raycast from
      // one of the upper corners of the hitbox.
      // (grown 1 just to make sure it doesnt interfere.)
      const Rectf eye = get_bbox().grown(1.f);
      if (!Sector::get().free_line_of_sight(
        vecdist.x <= 0 ? eye.p1() : Vector(eye.get_right(), eye.get_top()),
        obj->get_bbox().get_middle(),
        false,
        obj
      )) break;

      // Hop before fleeing.
      set_walk_speed(0);
      m_physic.set_velocity_y(HOP_HEIGHT);
      m_physic.set_velocity_x(0);
      m_physic.set_acceleration_x(0);
      m_dir = vecdist.x > 0 ? Direction::RIGHT : Direction::LEFT;
      m_sprite->set_action("flee", m_dir);
      m_state = GB_STATE_REALIZING;
      m_realize_timer.start(REALIZE_TIME);
      break;
    }

    case GB_STATE_REALIZING:
      if (!m_realize_timer.check())
        break;

      flee(vecdist.x > 0 ? Direction::LEFT : Direction::RIGHT);
      break;

    case GB_STATE_RECOVER:
      cornered();
      return;

    default:
      break;
  }
}

void GoldBomb::explode()
{
  MrBomb::explode();
  Sector::get().add<CoinExplode>(get_pos(), !m_parent_dispenser);
}

void
GoldBomb::flee(Direction dir)
{
  set_walk_speed(FLEEING_WALK_SPEED);
  set_ledge_behavior(LedgeBehavior::NORMAL);
  m_dir = dir;

  const float speed = FLEEING_WALK_SPEED * (m_dir == Direction::LEFT ? -1 : 1);
  m_physic.set_acceleration_x(speed);
  m_physic.set_velocity_x(speed);

  if (get_action() == dir_to_string(m_dir))
    m_sprite->set_animation_loops(-1);
  else
    set_action("flee", m_dir);

  m_state = GB_STATE_FLEEING;
}

void
GoldBomb::cornered()
{
  if (m_state == GB_STATE_CORNERED)
    return;

  set_walk_speed(0);
  m_physic.set_velocity_x(0);
  m_physic.set_acceleration_x(0);

  set_action("scared", m_dir);

  m_state = GB_STATE_CORNERED;
}

void
GoldBomb::recover()
{
  if (m_state == GB_STATE_RECOVER)
    return;

  set_action("recover", m_dir);

  m_state = GB_STATE_RECOVER;
}

void
GoldBomb::normalize()
{
  m_physic.set_velocity_x(NORMAL_WALK_SPEED * (m_dir == Direction::LEFT ? -1 : 1));
  m_physic.set_acceleration_x(0);

  set_action(m_dir);
  set_ledge_behavior(LedgeBehavior::SMART);
  set_walk_speed(NORMAL_WALK_SPEED);

  m_state = MB_STATE_NORMAL;
}

/* EOF */
