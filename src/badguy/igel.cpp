//  SuperTux - Badguy "Igel"
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//                2023 MatusGuy
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

#include "badguy/igel.hpp"

#include <variant>

#include "audio/sound_manager.hpp"
#include "collision/collision_system.hpp"
#include "math/easing.hpp"
#include "object/player.hpp"
#include "object/shard.hpp"
#include "supertux/sector.hpp"

namespace {

const float IGEL_NORMAL_SPEED = 80;
const float IGEL_CORRUPTED_SPEED = 120;

const float ROLL_RANGE = 32*10;
const float ROLL_SPEED = 350;
const float ROLL_DURATION = 2.f;
const float ROLL_EASE_TIMER = 0.5f;
const float ROLL_COOLDOWN = 1.f;

const std::string CORRUPTED_SHARD_SPRITE = "images/creatures/granito/corrupted/big/root_spike.sprite";

} // namespace

Igel::Igel(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/igel/igel.sprite", "left", "right"),
  m_state(STATE_NORMAL),
  m_roll_timer(),
  m_roll_cooldown(),
  m_ease_timer(),
  m_bonked(false)
{
  parse_type(reader);

  walk_speed = get_normal_walk_speed();
  set_ledge_behavior(LedgeBehavior::SMART);

  SoundManager::current()->preload("sounds/thud.ogg");
}

void
Igel::active_update(float dt_sec)
{
  WalkingBadguy::active_update(dt_sec);

  if (m_frozen) return;

  switch (m_state)
  {
    case STATE_CHARGING:
      if (m_sprite->animation_done())
      {
        roll();
      }

      break;

    case STATE_ROLLING:
      if (m_ease_timer.started())
      {
        float vel = (m_ease_timer.get_progress() * (ROLL_SPEED - get_normal_walk_speed())) + get_normal_walk_speed();
        set_walk_speed(vel);
        m_physic.set_velocity_x(vel * (m_dir == Direction::LEFT ? -1 : 1));
      }

      if (m_roll_timer.check())
      {
        if (should_roll())
        {
          // We're still tryna chase them?
          // Then continue chasing them.
          m_roll_timer.start(ROLL_DURATION);
          break;
        }

        stop_rolling();
        break;
      }

      break;

    case STATE_NORMAL:
      if (get_action() == "roll-end-" + dir_to_string(m_dir) &&
          m_sprite->animation_done())
      {
        set_action(m_dir);
      }

      if (m_ease_timer.started())
      {
        float vel = (m_ease_timer.get_progress() * (get_normal_walk_speed() - ROLL_SPEED)) + ROLL_SPEED;
        set_walk_speed(vel);
        m_physic.set_velocity_x(vel * (m_dir == Direction::LEFT ? -1 : 1));
      }

      if (m_bonked && m_ease_timer.check())
        set_action("roll-end", m_dir);

      if (!m_roll_cooldown.started() && should_roll()) charge();

      break;
  }
}

void
Igel::collision_solid(const CollisionHit &hit)
{
  WalkingBadguy::collision_solid(hit);

  if (m_state == STATE_ROLLING)
  {
    if ((hit.left && (m_dir == Direction::RIGHT)) || (hit.right && (m_dir == Direction::LEFT))) {
      stop_rolling(true);
      SoundManager::current()->play("sounds/thud.ogg");
    }
  }
}

HitResponse
Igel::collision_badguy(BadGuy &badguy, const CollisionHit &hit)
{
  if (m_state == STATE_ROLLING)
  {
    badguy.kill_fall();
    return FORCE_MOVE;
  }

  return WalkingBadguy::collision_badguy(badguy, hit);
}

bool
Igel::can_break() const
{
  return m_state == STATE_ROLLING;
}

void
Igel::run_dead_script()
{
  if (m_type == CORRUPTED)
  {
    Sector::get().add<Shard>(get_bbox().get_middle(), Vector(100.f, -500.f), CORRUPTED_SHARD_SPRITE);
    Sector::get().add<Shard>(get_bbox().get_middle(), Vector(270.f, -350.f), CORRUPTED_SHARD_SPRITE);
    Sector::get().add<Shard>(get_bbox().get_middle(), Vector(-100.f, -500.f),CORRUPTED_SHARD_SPRITE);
    Sector::get().add<Shard>(get_bbox().get_middle(), Vector(-270.f, -350.f),CORRUPTED_SHARD_SPRITE);
  }

  WalkingBadguy::run_dead_script();
}

void
Igel::unfreeze(bool melt)
{
  WalkingBadguy::unfreeze(melt);

  if (melt) return;

  m_state = STATE_NORMAL;
  m_bonked = false;

  set_action(m_dir);

  float vel = get_normal_walk_speed();
  set_walk_speed(vel);
  m_physic.set_velocity_x(vel * (m_dir == Direction::LEFT ? -1 : 1));

  m_roll_timer.stop();
  m_ease_timer.stop();
}

GameObjectTypes
Igel::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "corrupted", _("Corrupted") }
  };
}

std::string
Igel::get_default_sprite_name() const
{
  switch (m_type)
  {
    case CORRUPTED:
      return "images/creatures/igel/corrupted/corrupted_igel.sprite";
    default:
      return "images/creatures/igel/igel.sprite";
  }
}

bool
Igel::should_roll() const
{
  Player* player = get_nearest_player();
  if (!player) return false;

  Rectf player_box = player->get_bbox();

  bool in_reach_left = (player_box.get_right() >= get_bbox().get_right()-((m_dir == Direction::LEFT) ? ROLL_RANGE : 0));
  bool in_reach_right = (player_box.get_left() <= get_bbox().get_left()+((m_dir == Direction::RIGHT) ? ROLL_RANGE : 0));
  bool in_reach_top = (player_box.get_bottom() >= get_bbox().get_top());
  bool in_reach_bottom = (player_box.get_top() <= get_bbox().get_bottom());

  Rectf box = get_bbox().grown(1.f);
  Vector eye = {m_dir == Direction::LEFT ? box.get_left() : box.get_right(), box.get_middle().y};
  bool can_see_player = Sector::get().free_line_of_sight(eye, player_box.get_middle(), true);

  return in_reach_left && in_reach_right && in_reach_top && in_reach_bottom && can_see_player;
}

void
Igel::charge()
{
  m_state = STATE_CHARGING;
  //TODO: Add an audio cue!!
  set_action("roll-start", m_dir);
  set_walk_speed(0.f);
  m_physic.set_velocity_x(0.f);
}

void
Igel::roll()
{
  m_state = STATE_ROLLING;

  set_action("roll", m_dir);

  set_ledge_behavior(LedgeBehavior::FALL);

  m_roll_timer.start(ROLL_DURATION);
  m_ease_timer.start(ROLL_EASE_TIMER);
}

void
Igel::stop_rolling(bool bonk)
{
  m_state = STATE_NORMAL;
  m_bonked = bonk;

  set_action(m_bonked ? "roll" : "roll-end", m_dir);

  if (m_bonked)
  {
    // Hop a little
    m_physic.set_velocity_y(-250.f);
  }

  set_ledge_behavior(LedgeBehavior::SMART);

  m_roll_timer.stop();
  m_roll_cooldown.start(ROLL_COOLDOWN);
  m_ease_timer.start(ROLL_EASE_TIMER);
}

float
Igel::get_normal_walk_speed() const
{
  return m_type == CORRUPTED ? IGEL_CORRUPTED_SPEED : IGEL_NORMAL_SPEED;
}

/* EOF */
