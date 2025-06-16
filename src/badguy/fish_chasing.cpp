//  SuperTux
//  Copyright (C) 2022 Daniel Ward <weluvgoatz@gmail.com>
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

#include "badguy/fish_chasing.hpp"

#include "object/player.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"
#include "util/reader_mapping.hpp"

static const float TRACK_DISTANCE = 200.0f;
static const float LOST_DISTANCE = 400.0f;
static const float CHASE_SPEED = 300.0f;
static const float REALIZATION_TIME = 0.7f;
static const float ACCELERATION_TIME = 0.2f;
static const float DECELERATION_TIME = 0.4f;

FishChasing::FishChasing(const ReaderMapping& reader) :
  FishSwimming(reader, "images/creatures/fish/ice/greenfish.sprite"),
  m_chase_state(ChaseState::NORMAL),
  m_realization_timer(),
  m_acceleration_timer(),
  m_is_accelerating(false),
  m_last_chase_velocity(),
  m_track_distance(),
  m_lost_distance(),
  m_chase_speed()
{
  parse_type(reader);
  reader.get("track-distance", m_track_distance, TRACK_DISTANCE);
  reader.get("lost-distance", m_lost_distance, LOST_DISTANCE);
  reader.get("chase-speed", m_chase_speed, CHASE_SPEED);
}

std::string
FishChasing::get_default_sprite_name() const
{
  switch (m_type)
  {
    case FOREST:
      return "images/creatures/fish/forest/brownfish.sprite";
    case CORRUPTED:
      return "images/creatures/fish/forest/corrupted/corrupted_brownfish.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
FishChasing::active_update(float dt_sec) {
  // Perform basic updates.
  BadGuy::active_update(dt_sec);
  m_in_water = !Sector::get().is_free_of_tiles(get_bbox(), true, Tile::WATER);

  Rectf top_of_fish = get_bbox();
  top_of_fish.set_height(1.0f);

  const bool is_completely_in_water = !Sector::get().is_free_of_tiles(top_of_fish, true, Tile::WATER);
  m_physic.enable_gravity(m_frozen || !is_completely_in_water);

  // Handle beached state when the fish is in water and beached_timer is active.
  if (m_in_water && m_beached_timer.started())
    m_beached_timer.stop();

  if (m_beached_timer.check())
  {
    ignite();
    m_physic.reset();
    m_beached_timer.stop();
  }

  // Behavior - chase the nearest player.
  auto player = get_nearest_player();
  if (!player) return;
  const Vector p1 = m_col.m_bbox.get_middle();
  const Vector p2 = player->get_bbox().get_middle();
  const Vector dist = (p2 - p1);
  const bool is_player_in_water = player->is_swimming() || player->is_swimboosting() || player->is_water_jumping();
  const bool is_facing_player = (m_dir == Direction::LEFT && dist.x <= 0.0f)
                             || (m_dir == Direction::RIGHT && dist.x >= 0.0f )
                             || (m_dir == Direction::UP && dist.y <= 0.0f )
                             || (m_dir == Direction::DOWN && dist.y >= 0.0f );
  const bool can_see_player = Sector::get().free_line_of_sight(p1,p2, true, this);

  switch (m_chase_state)
  {
  case NORMAL:
    FishSwimming::active_update(dt_sec);

    if (glm::length(dist) <= m_track_distance && m_in_water && !m_frozen && is_player_in_water && is_facing_player && can_see_player)
    {
      m_realization_timer.start(REALIZATION_TIME);
      m_chase_state = ChaseState::FOUND;
    }
    break;
  case FOUND:
    if (!m_frozen)
    {
      Direction dir = m_physic.get_velocity_x() <= 0.f ? Direction::LEFT : Direction::RIGHT;
      set_action("notice", dir, 1);
    }

    if (std::abs(glm::length(m_physic.get_velocity())) >= 1.f) {
      m_physic.set_velocity(m_physic.get_velocity() / 1.25f);
    }

    if (m_realization_timer.check())
    {
      m_realization_timer.stop();
      m_is_accelerating = true;
      m_acceleration_timer.start(ACCELERATION_TIME);
      m_chase_state = ChaseState::CHASING;
    }
    break;
  case CHASING:
    if (glm::length(dist) > m_lost_distance || !is_player_in_water || !can_see_player)
    {
      m_last_chase_velocity = glm::length(m_physic.get_velocity());
      m_acceleration_timer.start(DECELERATION_TIME);
      m_is_accelerating = true;
      m_chase_state = ChaseState::LOST;
    }
    else if (glm::length(dist) >= 1 && m_in_water && !m_frozen)
    {
      const Vector dir = glm::normalize(dist);
      float swim_velocity = m_chase_speed;

      if (m_acceleration_timer.check())
      {
        m_is_accelerating = false;
      }

      if (m_is_accelerating)
      {
        const float acceleration_progress = m_acceleration_timer.get_timegone() / ACCELERATION_TIME;
        swim_velocity = acceleration_progress * m_chase_speed;
      }
      m_physic.set_velocity(dir * swim_velocity);
    }
    else
    {
      /* We somehow landed right on top of the player without colliding.
       * Sit tight and avoid a division by zero. */
    }
    m_dir = (m_physic.get_velocity_x() <= 0.f ? Direction::LEFT : Direction::RIGHT);
    if (!m_frozen)
      set_action("chase", m_dir);
    break;
  case LOST:
    if (!m_frozen)
      set_action("swim", m_dir);

    if (m_in_water && !m_frozen)
    {
      if (m_is_accelerating)
      {
        if (m_acceleration_timer.check())
        {
          m_is_accelerating = false;
          m_start_position = get_pos();
          m_state = FishYState::BALANCED;
          m_chase_state = ChaseState::NORMAL;
          m_physic.set_velocity(0.f, 0.f);
        }
        else
        {
          const float swim_velocity = m_last_chase_velocity * (m_acceleration_timer.get_timeleft() / DECELERATION_TIME);
          m_physic.set_velocity(glm::normalize(m_physic.get_velocity()) * swim_velocity);
        }
      }
    }
    break;
  }
}

HitResponse
FishChasing::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (m_chase_state == ChaseState::CHASING)
  {
    badguy.kill_fall();
    return FORCE_MOVE;
  }

  return FishSwimming::collision_badguy(badguy, hit);
}

ObjectSettings
FishChasing::get_settings()
{
  ObjectSettings result = FishSwimming::get_settings();

  result.add_float(_("Tracking Distance"), &m_track_distance, "track-distance", m_track_distance);
  result.add_float(_("Losing Distance"), &m_lost_distance, "lost-distance", m_lost_distance);
  result.add_float(_("Chase Speed"), &m_chase_speed, "chase-speed", m_chase_speed);

  result.reorder({ "radius", "direction", "track-distance", "lost-distance", "chase-speed", "x", "y" });

  return result;
}

/* EOF */
