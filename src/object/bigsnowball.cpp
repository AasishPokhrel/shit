//  Copyright (C) 2024 Daniel Ward <weluvgoatz@gmail.com>
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

#include "object/bigsnowball.hpp"

#include "badguy/badguy.hpp"
#include "badguy/yeti.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "object/bumper.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "object/trampoline.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"
#include "util/reader_mapping.hpp"

namespace {
  const float SPEED_X = 350.f;
  const float SPEED_Y = -300.f;
}

BigSnowball::BigSnowball(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/big_snowball/big_snowball.sprite", LAYER_OBJECTS, COLGROUP_MOVING_STATIC),
  m_physic(),
  m_dir(Direction::LEFT),
  m_speed(),
  m_break_on_impact(),
  m_bounce()
{
  reader.get("speed", m_speed, SPEED_X);
  reader.get("break-on-impact", m_break_on_impact, false);
  reader.get("bounce", m_bounce, true);
  std::string dir_str;
  if (reader.get("direction", dir_str)) {
    m_dir = string_to_dir(dir_str);
  }
  m_physic.set_velocity_x(m_dir == Direction::RIGHT ? m_speed : -m_speed);
  set_action("default", m_dir);
}

BigSnowball::BigSnowball(const Vector& pos, const Direction& dir, bool bounce) :
  MovingSprite(pos, "images/objects/big_snowball/big_snowball.sprite", LAYER_OBJECTS, COLGROUP_MOVING_STATIC),
  m_physic(),
  m_dir(Direction::LEFT),
  m_speed(),
  m_break_on_impact(),
  m_bounce(bounce)
{
  // settings used by Yeti when it throws it
  m_dir = dir;
  m_speed = SPEED_X;
  m_break_on_impact = true;
  m_physic.set_velocity_x(m_dir == Direction::RIGHT ? SPEED_X : -SPEED_X);
  if (bounce) {
    m_physic.set_velocity_y(SPEED_Y);
  }
  set_action("default", m_dir);
}

ObjectSettings
BigSnowball::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_direction(_("Direction"), &m_dir, { Direction::RIGHT, Direction::LEFT }, "direction");
  result.add_float(_("Speed"), &m_speed, "speed", SPEED_X);
  result.add_bool(_("Break on impact?"), &m_break_on_impact, "break-on-impact", false);
  result.add_bool(_("Bounce?"), &m_bounce, "bounce", true);
  result.reorder({ "direction", "speed", "break-on-impact", "bounce", "sprite", "x", "y" });
  return result;
}

void
BigSnowball::update(float dt_sec)
{
  if (get_bbox().get_top() > Sector::get().get_height()) {
    remove_me();
  }

  // Left-right faux collision

  Rectf side_look_box = get_bbox().grown(-1.f);
  side_look_box.set_left(get_bbox().get_left() + (m_dir == Direction::LEFT ? -1.f : 1.f));
  side_look_box.set_right(get_bbox().get_right() + (m_dir == Direction::LEFT ? -1.f : 1.f));
  if (!Sector::get().is_free_of_statics(side_look_box))
  {
    if (m_break_on_impact) {
      spawn_particles();
    }
    else
    {
      m_dir = m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
      set_action(m_dir);
      m_physic.set_velocity_x(-m_physic.get_velocity_x());
    }
  }

  Rectf spikebox = get_bbox().grown(-6.f);
  spikebox.set_bottom(get_bbox().get_bottom() - (m_physic.get_velocity_y() > 300.f ? 6.f : 40.f));
  if (!Sector::get().is_free_of_tiles(spikebox, false, Tile::HURTS)) {
    spawn_particles();
  }

  bool in_water = !Sector::get().is_free_of_tiles(get_bbox(), true, Tile::WATER);

  Vector movement = m_physic.get_movement(dt_sec) * Vector(in_water ? 0.4f : 1.f, in_water ? 0.6f : 1.f);
  m_sprite->set_angle(m_sprite->get_angle() + movement.x * 3.141592653898f / 2.f);
  m_col.set_movement(movement);
  //m_col.propagate_movement(movement);
}

HitResponse
BigSnowball::collision(MovingObject& other, const CollisionHit& hit)
{
  // ignore collisions with yeti
  auto yeti = dynamic_cast<Yeti*>(&other); // cppcheck-suppress constVariablePointer
  if (yeti) {
    return ABORT_MOVE;
  }

  auto badguy = dynamic_cast<BadGuy*>(&other); // cppcheck-suppress constVariablePointer
  if (badguy && ((m_dir == Direction::LEFT && hit.left) || (m_dir == Direction::RIGHT && hit.right) || hit.bottom))
  {
    badguy->kill_fall();
    return ABORT_MOVE;
  }

  auto player = dynamic_cast<Player*>(&other); // cppcheck-suppress constVariablePointer
  if (player && player->m_does_buttjump && hit.top)
  {
    spawn_particles();
    return ABORT_MOVE;
  }

  auto bs = dynamic_cast<BigSnowball*>(&other); // cppcheck-suppress constVariablePointer
  if (bs) {
    collision_solid(hit);
    return ABORT_MOVE;
  }

  auto tramp = dynamic_cast<Trampoline*>(&other); // cppcheck-suppress constVariablePointer
  if (tramp)
  {
    m_physic.set_velocity_y(-500.f);
    tramp->bounce();
    return ABORT_MOVE;
  }

  auto bumper = dynamic_cast<Bumper*>(&other); // cppcheck-suppress constVariablePointer
  if (bumper)
  {
    m_physic.set_velocity_x(-m_physic.get_velocity_x());
    m_dir = m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
    bumper->bounce();
    return ABORT_MOVE;
  }

  return FORCE_MOVE;
}

void
BigSnowball::collision_solid(const CollisionHit& hit)
{
  if (hit.bottom)
  {
    if (m_physic.get_velocity_y() > 10.f && m_bounce) {
      m_physic.set_velocity_y(-std::pow(m_physic.get_velocity_y(), (9.f / 10.f)));
    }
    else {
      m_physic.set_velocity_y(0.f);
    }
  }

  if ((hit.left || hit.right) && m_break_on_impact) {
    spawn_particles();
  }

  if (!m_break_on_impact)
  {
    if (m_dir == Direction::LEFT && hit.left)
    {
      m_physic.set_velocity_x(m_speed);
      m_dir = Direction::RIGHT;
    }
    if (m_dir == Direction::RIGHT && hit.right)
    {
      m_physic.set_velocity_x(-m_speed);
      m_dir = Direction::LEFT;
    }
  }
}

void
BigSnowball::spawn_particles()
{
  for (int i = 0; i < 8; i++)
  {
    Sector::get().add<SpriteParticle>(m_sprite_name, "particle",
      get_bbox().get_middle() + (15.f * Vector(std::cos(math::PI_4*static_cast<float>(i)), std::sin(math::PI_4*static_cast<float>(i)))),
      ANCHOR_MIDDLE, (150.f * (glm::normalize(Vector(std::cos(math::PI_4*static_cast<float>(i)), std::sin(math::PI_4*static_cast<float>(i)))))) +
                      Vector(gameRandom.randf(-40.f, 40.f), gameRandom.randf(-40.f, 40.f)),
      Vector(0.f, 1000.f), LAYER_OBJECTS + 1, true);
  }
  remove_me();
}

/* EOF */
