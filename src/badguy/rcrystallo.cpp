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

#include "badguy/rcrystallo.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/crystallo.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "object/shard.hpp"
#include "supertux/sector.hpp"

#include "util/reader_mapping.hpp"

RCrystallo::RCrystallo(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/crystallo/crystallo.sprite", "left", "right"),
  m_state(RCRYSTALLO_ROOF),
  m_radius()
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
  reader.get("radius", m_radius, 100.0f);
  SoundManager::current()->preload("sounds/crystallo-shatter.ogg");
}

RCrystallo::RCrystallo(const Vector& pos, const Vector& start_pos, float vel_x, std::unique_ptr<Sprite> sprite,
                       Direction dir, float radius, const std::string& script, bool fall) :
  WalkingBadguy(pos, dir, "images/creatures/crystallo/crystallo.sprite", "left", "right"),
  m_state(RCRYSTALLO_ROOF),
  m_radius(radius)
{
  if (fall)
  {
    m_state = RCRYSTALLO_DETECT;
    m_physic.set_gravity_modifier(0.f);
  }
  else
  {
    m_physic.set_gravity_modifier(-1.f);
  }
  m_physic.set_velocity_x(vel_x);
  m_sprite = std::move(sprite);
  m_dead_script = script;
  m_start_position = start_pos;
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
  SoundManager::current()->preload("sounds/crystallo-shatter.ogg");
}

void
RCrystallo::initialize()
{
  Rectf magnetic_box = get_bbox();
  magnetic_box.set_top(m_col.m_bbox.get_top() - 80.f);
  if (m_state != RCRYSTALLO_DETECT)
  {
    m_state = Sector::get().is_free_of_statics(magnetic_box) ? RCRYSTALLO_FALLING : RCRYSTALLO_ROOF;
  }
  else
  {
    m_col.m_bbox.move(Vector(3.f, 0.f));
    set_action(m_dir == Direction::LEFT ? "roof-detected-left" : "roof-detected-right", 1, ANCHOR_TOP);
  }
}

ObjectSettings
RCrystallo::get_settings()
{
  ObjectSettings result = WalkingBadguy::get_settings();
  result.add_float(_("Radius"), &m_radius, "radius", 100.0f);
  result.reorder({ "radius", "direction", "x", "y" });
  return result;
}

void
RCrystallo::active_update(float dt_sec)
{
  // Initialization of certain events prior to going into cases.
  auto player = get_nearest_player();
  float targetwalk = m_dir == Direction::LEFT ? -80.f : 80.f;
  Rectf reversefallbox = get_bbox();

  switch (m_state)
  {
  case RCRYSTALLO_ROOF:
    m_physic.set_gravity_modifier(-1.f);
    // Walking and turning properly.
      if (m_dir != Direction::LEFT && get_pos().x > (m_start_position.x + m_radius - 20.f))
        targetwalk = -80.f;
      if (m_dir != Direction::RIGHT && get_pos().x < (m_start_position.x - m_radius + 20.f))
        targetwalk = 80.f;
      set_action(std::abs(m_physic.get_velocity_x()) < 80.f ?
        m_dir == Direction::LEFT ? "roof-slowdown-left" : "roof-slowdown-right" :
        m_dir == Direction::LEFT ? "roof-left" : "roof-right", -1);
    // Turn at holes.
    reversefallbox.set_top(m_col.m_bbox.get_top() - 33.f);
    reversefallbox.set_left(m_col.m_bbox.get_left() + (m_dir == Direction::LEFT ? -5.f : 34.f));
    reversefallbox.set_right(m_col.m_bbox.get_right() + (m_dir == Direction::LEFT ? -34.f : 5.f));
    if (Sector::get().is_free_of_statics(reversefallbox))
      turn_around();
    // Detect player and fall when it is time.
    if (player && player->get_bbox().get_right() > m_col.m_bbox.get_left() - 192.f
      && player->get_bbox().get_left() < m_col.m_bbox.get_right() + 192.f
      && player->get_bbox().get_bottom() > m_col.m_bbox.get_top()
      && Sector::get().free_line_of_sight(m_col.m_bbox.get_middle() + Vector(0, 20),
        player->get_bbox().get_middle() - Vector(0, 40), false, player))
    {
      // Center enemy, begin falling.
      m_col.m_bbox.move(Vector(3.f, 0.f));
      set_action(m_dir == Direction::LEFT ? "roof-detected-left" : "roof-detected-right", 1, ANCHOR_TOP);
      m_state = RCRYSTALLO_DETECT;
    }
    WalkingBadguy::active_update(dt_sec, targetwalk, 2.f);
    break;
  case RCRYSTALLO_DETECT:
    m_physic.set_velocity(0.f, 0.f);
    m_physic.set_gravity_modifier(0.f);
    m_physic.set_acceleration(0.f, 0.f);
    if (m_sprite->animation_done())
    {

      m_physic.set_gravity_modifier(1.f);
      set_action(m_dir == Direction::LEFT ? "roof-fall-left" : "roof-fall-right", 1, ANCHOR_TOP);
      m_state = RCRYSTALLO_FALLING;
    }
    BadGuy::active_update(dt_sec);
    break;
  case RCRYSTALLO_FALLING:
    BadGuy::active_update(dt_sec);
    break;
  }
}

void
RCrystallo::draw(DrawingContext& context)
{
  context.push_transform();
  Vector draw_pos = get_pos() + m_physic.get_velocity() * context.get_time_offset();
  m_sprite->draw(context.color(), draw_pos, m_layer);
  context.pop_transform();
}

void
RCrystallo::collision_solid(const CollisionHit& hit)
{
  if (m_state == RCRYSTALLO_FALLING && hit.bottom)
    kill_fall();
  WalkingBadguy::collision_solid(hit);
}

HitResponse
RCrystallo::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (m_state == RCRYSTALLO_FALLING)
  {
    badguy.kill_fall();
    kill_fall();
  }
  return WalkingBadguy::collision_badguy(badguy, hit);
}

HitResponse
RCrystallo::collision_player(Player& player, const CollisionHit& hit)
{
  if (m_state == RCRYSTALLO_FALLING)
    kill_fall();
  return BadGuy::collision_player(player, hit);
}

bool
RCrystallo::is_flammable() const
{
  return false;
}

void
RCrystallo::kill_fall()
{
  m_physic.set_gravity_modifier(1.f);
  if (m_state == RCRYSTALLO_FALLING)
  {
    SoundManager::current()->play("sounds/crystallo-shatter.ogg", get_pos());
    if (is_valid())
    {
      remove_me();
      // Create 4 shards that the enemy splits into, which serve as an additional threat.
      Sector::get().add<Shard>(m_col.m_bbox.get_middle(), Vector(100.f, -500.f));
      Sector::get().add<Shard>(m_col.m_bbox.get_middle(), Vector(270.f, -350.f));
      Sector::get().add<Shard>(m_col.m_bbox.get_middle(), Vector(-100.f, -500.f));
      Sector::get().add<Shard>(m_col.m_bbox.get_middle(), Vector(-270.f, -350.f));
    }
    run_dead_script();
  }
  else
    BadGuy::kill_fall();
}

void
RCrystallo::after_editor_set()
{
  WalkingBadguy::after_editor_set();

  set_action("roof", m_start_dir == Direction::AUTO ? Direction::LEFT : m_start_dir);
  update_hitbox();
}

void
RCrystallo::on_flip(float height)
{
  WalkingBadguy::on_flip(height);

  Sector::get().add<Crystallo>(get_pos(), m_start_position, get_velocity_x(),
                               std::move(m_sprite), m_dir, m_radius, m_dead_script);
  remove_me();
}

/* EOF */
