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

#include "badguy/flyingsnowball.hpp"

#include "math/random.hpp"
#include "math/util.hpp"
#include "object/sprite_particle.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"

namespace {
const float PUFF_INTERVAL_MIN = 4.0f; /**< Spawn new puff of smoke at most that often. */
const float PUFF_INTERVAL_MAX = 8.0f; /**< Spawn new puff of smoke at least that often. */
const float GLOBAL_SPEED_MULT = 0.8f; /**< The overall movement speed/rate. */
}

FlyingSnowBall::FlyingSnowBall(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/flying_snowball/flying_snowball.sprite"),
  total_time_elapsed(),
  puff_timer()
{
  m_physic.enable_gravity(false);
}

void
FlyingSnowBall::initialize()
{
  set_action(m_dir);
}

void
FlyingSnowBall::activate()
{
  puff_timer.start(static_cast<float>(gameRandom.randf(PUFF_INTERVAL_MIN, PUFF_INTERVAL_MAX)));
}

bool
FlyingSnowBall::collision_squished(MovingObject& object)
{
  set_action("squished", m_dir);
  m_physic.enable_gravity(true);
  m_physic.set_acceleration_y(0);
  m_physic.set_velocity_y(0);
  kill_squished(object);
  return true;
}

void
FlyingSnowBall::collision_solid(const CollisionHit& hit)
{
  if (hit.top || hit.bottom) {
    m_physic.set_velocity_y(0);
  }
}

void
FlyingSnowBall::active_update(float dt_sec)
{
  total_time_elapsed = fmodf(total_time_elapsed + dt_sec, math::TAU / GLOBAL_SPEED_MULT);

  float delta = total_time_elapsed * GLOBAL_SPEED_MULT;

  // Put that function in a graphing calculator :
  // sin(x)^3 + sin(3(x - pi/3))/3
  float targetHgt = std::pow(std::sin(delta), 3.f) +
                    std::sin(3.f *
                             ((delta - math::PI) / 3.f)
                            ) / 3.f;
  targetHgt = targetHgt * 100.f + m_start_position.y;
  m_physic.set_velocity_y(targetHgt - get_pos().y);

  m_col.set_movement(m_physic.get_movement(1.f));

  auto player = get_nearest_player();
  if (player) {
    m_dir = (player->get_pos().x > get_pos().x) ? Direction::RIGHT : Direction::LEFT;
    set_action(m_dir);
  }

  // Spawn smoke puffs.
  if (puff_timer.check()) {
    Vector ppos = m_col.m_bbox.get_middle();
    Vector pspeed = Vector(graphicsRandom.randf(-10, 10), 150);
    Vector paccel = Vector(0,0);
    Sector::get().add<SpriteParticle>("images/particles/smoke.sprite",
                                           "default",
                                           ppos, ANCHOR_MIDDLE, pspeed, paccel,
                                           LAYER_OBJECTS-1);
    puff_timer.start(gameRandom.randf(PUFF_INTERVAL_MIN, PUFF_INTERVAL_MAX));
  }

}

std::vector<Direction>
FlyingSnowBall::get_allowed_directions() const
{
  return {};
}

/* EOF */
