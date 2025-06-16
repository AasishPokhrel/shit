//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#include "badguy/dive_mine.hpp"

#include "editor/editor.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

const float DiveMine::s_trigger_radius = 100.f;
const float DiveMine::s_swim_speed = 20.f;
const float DiveMine::s_max_float_acceleration = 15.f;

DiveMine::DiveMine(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/dive_mine/dive_mine.sprite"),
  m_ticking_glow(SpriteManager::current()->create("images/creatures/dive_mine/ticking_glow/ticking_glow.sprite")),
  m_chasing(true)
{
  reset_sprites();
  m_water_affected = false;
}

void
DiveMine::reset_sprites()
{
  set_action(m_dir);
  m_ticking_glow->set_action("idle");
}

void
DiveMine::stop_chasing()
{
  if (!m_chasing)
    return;

  m_physic.reset();
  m_physic.set_velocity_y(1.f);
  m_physic.set_acceleration_y(s_max_float_acceleration);

  reset_sprites();
  m_chasing = false;
}

void
DiveMine::explode()
{
  remove_me();
  Sector::get().add<Explosion>(m_col.m_bbox.get_middle(), EXPLOSION_STRENGTH_DEFAULT);
  run_dead_script();
}

void
DiveMine::collision_solid(const CollisionHit& hit)
{
  if (!m_frozen) {
    if (m_in_water)
    {
      if (hit.left || hit.right)
        turn_around();
    }
    else
    {
      explode();
    }
  }
  else {
    BadGuy::collision_solid(hit);
  }
}

HitResponse
DiveMine::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (!m_frozen &&
      ((hit.left && (m_dir == Direction::LEFT)) || (hit.right && (m_dir == Direction::RIGHT))))
  {
    turn_around();
  }

  BadGuy::collision_badguy(badguy, hit);
  return CONTINUE;
}

HitResponse
DiveMine::collision_player(Player& player, const CollisionHit& hit)
{
  if (!m_frozen)
    explode();

  return FORCE_MOVE;
}

void
DiveMine::kill_fall()
{
  explode();
}

void
DiveMine::draw(DrawingContext& context)
{
  BadGuy::draw(context);

  if (m_frozen || Editor::is_active())
    return;

  m_ticking_glow->set_blend(Blend::ADD);
  m_ticking_glow->draw(context.light(),
                       Vector(m_col.m_bbox.get_left() + m_col.m_bbox.get_width() / 2,
                              m_col.m_bbox.get_top() - 8.f),
                       m_layer, m_flip);
}

void
DiveMine::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);

  if (m_frozen || !m_in_water)
  {
    m_physic.enable_gravity(true);
    return;
  }
  m_physic.enable_gravity(false);

  // Update float cycles.
  if (!m_chasing)
  {
    if (std::abs(m_physic.get_acceleration_y()) > s_max_float_acceleration * 3)
      m_physic.inverse_velocity_y();

    m_physic.set_acceleration_y(m_physic.get_acceleration_y() + (s_max_float_acceleration / 25) * (m_physic.get_velocity_y() < 0.f ? 1 : -1));
  }

  // Detect if player is near.
  auto player = get_nearest_player();
  if (player)
  {
    // Face the player.
    m_dir = (player->get_pos().x > get_pos().x) ? Direction::RIGHT : Direction::LEFT;
  }

  if (!player || !player->is_swimming())
  {
    stop_chasing();
    return;
  }

  Vector dist = player->get_bbox().get_middle() - m_col.m_bbox.get_middle();
  if (m_chasing)
  {
    if (glm::length(dist) > s_trigger_radius) // Player is out of trigger radius.
    {
      stop_chasing();
      return;
    }

    set_action("ticking", m_dir);
    m_ticking_glow->set_action("ticking");

    m_physic.set_velocity(glm::normalize(dist) * s_swim_speed);
  }
  else
  {
    set_action(m_dir);
    m_chasing = (glm::length(dist) <= s_trigger_radius);
  }
}

void
DiveMine::ignite()
{
  explode();
}

void
DiveMine::freeze()
{
  BadGuy::freeze();

  m_physic.reset();
  m_physic.set_velocity_y(1.f);
}

void
DiveMine::unfreeze(bool melt)
{
  BadGuy::unfreeze();

  m_chasing = true; // Ensure stop_chasing() will be executed.
  m_in_water = false;
  stop_chasing();
}

void
DiveMine::turn_around()
{
  if (m_frozen || m_chasing)
    return;

  m_dir = (m_dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT);
}

std::vector<Direction>
DiveMine::get_allowed_directions() const
{
  return {};
}

/* EOF */
