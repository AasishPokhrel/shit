//  SuperTux
//  Copyright (C) 2021 Daniel Ward <weluvgoatz@gmail.com>
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

#include "badguy/scrystallo.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/rcrystallo.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

SCrystallo::SCrystallo(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/crystallo/crystallo.sprite", "sleeping-left", "sleeping-right"),
  m_state(SCRYSTALLO_SLEEPING),
  m_roof(),
  m_radius(),
  m_range(),
  m_radius_anchor()
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
  reader.get("roof", m_roof, false);
  reader.get("radius", m_radius, 100.0f);
  reader.get("range", m_range, 250.0f);
  SoundManager::current()->preload("sounds/crystallo-pop.ogg");
}

void
SCrystallo::initialize()
{
  m_physic.enable_gravity(false);
  if (m_roof) FlipLevelTransformer::transform_flip(m_flip);
  m_state = SCRYSTALLO_SLEEPING;
  set_action("sleeping", m_dir);
}

ObjectSettings
SCrystallo::get_settings()
{
  ObjectSettings result = WalkingBadguy::get_settings();

  result.add_float(_("Walk Radius"), &m_radius, "radius", 100.0f);
  result.add_float(_("Awakening Radius"), &m_range, "range", 250.0f);
  result.add_bool(_("Roof-attached"), &m_roof, "roof", false);

  result.reorder({ "radius", "range", "direction", "x", "y" });

  return result;
}

void
SCrystallo::collision_solid(const CollisionHit& hit)
{
  if (m_state != SCRYSTALLO_WALKING)
  {
    BadGuy::collision_solid(hit);
    return;
  }
  WalkingBadguy::collision_solid(hit);
}

HitResponse
SCrystallo::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (m_state != SCRYSTALLO_WALKING)
  {
    return BadGuy::collision_badguy(badguy, hit);
  }
  return WalkingBadguy::collision_badguy(badguy, hit);
}

void
SCrystallo::active_update(float dt_sec)
{
  auto player = get_nearest_player();
  Rectf downbox = get_bbox();
  switch (m_state)
  {
  case SCRYSTALLO_SLEEPING:
    m_physic.set_velocity(0.f, 0.f);
    m_physic.set_acceleration(0.f, 0.f);
    // The entity is sleeping peacefully.
    if (player)
    {
      Vector p1 = m_col.m_bbox.get_middle();
      Vector p2 = player->get_bbox().get_middle();
      Vector dist = (p2 - p1);
      if (glm::length(dist) <= m_range)
      {
        set_action("waking", m_dir, 1);
        m_state = SCRYSTALLO_WAKING;
      }
    }
    BadGuy::active_update(dt_sec);
    break;
  case SCRYSTALLO_WAKING:
    m_physic.set_velocity(0.f, 0.f);
    m_physic.set_acceleration(0.f, 0.f);
    // Wake up and acknowledge surroundings once the animation is done.
    if (m_sprite->animation_done())
    {
      SoundManager::current()->play("sounds/crystallo-pop.ogg", get_pos());

      if (m_flip == VERTICAL_FLIP)
      {
        Sector::get().add<RCrystallo>(get_pos(), m_start_position, get_velocity_x(),
                                      std::move(m_sprite), m_dir, m_radius, m_dead_script, true);
        remove_me();
        return;
      }

      m_physic.enable_gravity(true);
      m_physic.set_velocity_y(-250.f);
      WalkingBadguy::initialize();
      set_action("jumping", m_dir, -1);
      m_state = SCRYSTALLO_JUMPING;
    }
    BadGuy::active_update(dt_sec);
    break;
  case SCRYSTALLO_JUMPING:
    // Popping out of the hole, ends when near the ground.
    downbox.set_bottom(get_bbox().get_bottom() + 10.f);
    if (!Sector::get().is_free_of_statics(downbox))
    {
      m_radius_anchor = get_pos();
      m_state = SCRYSTALLO_WALKING;
    }
    WalkingBadguy::active_update(dt_sec);
    break;
  case SCRYSTALLO_WALKING:
    // Walking and turning properly.
    float targetwalk = m_dir == Direction::LEFT ? -80.f : 80.f;
    if (m_dir != Direction::LEFT && get_pos().x > (m_radius_anchor.x + m_radius - 20.f))
      targetwalk = -80.f;
    if (m_dir != Direction::RIGHT && get_pos().x < (m_radius_anchor.x - m_radius + 20.f))
      targetwalk = 80.f;
    set_action(std::abs(m_physic.get_velocity_x()) < 80.f ?
      m_dir == Direction::LEFT ? "slowdown-left" : "slowdown-right" :
      m_dir == Direction::LEFT ? "left" : "right", -1);
    WalkingBadguy::active_update(dt_sec, targetwalk, 2.f);
    break;
	}
}

bool
SCrystallo::collision_squished(MovingObject& object)
{
  set_action(m_dir == Direction::LEFT ? "shattered-left" : "shattered-right", /* loops = */ -1, ANCHOR_BOTTOM);
  kill_squished(object);
  m_physic.set_velocity_x(0.0);
  m_physic.set_acceleration_x(0.0);
  return true;
}

bool
SCrystallo::is_flammable() const
{
  return false;
}

void
SCrystallo::after_editor_set()
{
  WalkingBadguy::after_editor_set();

  if ((m_roof && m_flip == NO_FLIP) || (!m_roof && m_flip == VERTICAL_FLIP))
    FlipLevelTransformer::transform_flip(m_flip);
  set_action("sleeping", m_start_dir == Direction::AUTO ? Direction::LEFT : m_start_dir);
  update_hitbox();
}

void
SCrystallo::on_flip(float height)
{
  WalkingBadguy::on_flip(height);

  if (m_state == SCRYSTALLO_SLEEPING || m_state == SCRYSTALLO_WAKING)
  {
    FlipLevelTransformer::transform_flip(m_flip);
  }
  else
  {
    Sector::get().add<RCrystallo>(get_pos(), m_start_position, get_velocity_x(),
                                  std::move(m_sprite), m_dir, m_radius, m_dead_script);
    remove_me();
  }
}

/* EOF */
