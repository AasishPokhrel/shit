//  AngryStone - A spiked block that charges towards the player
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

#include "badguy/angrystone.hpp"

#include "object/player.hpp"
#include "sprite/sprite.hpp"

static const float CHARGE_SPEED = 240;
static const float CHARGE_TIME = .5;
static const float ATTACK_TIME = 1;
static const float RECOVER_TIME = .5;

AngryStone::AngryStone(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/angrystone/angrystone.sprite"),
  m_attack_direction(0.0f, 0.0f),
  m_old_wall_direction(0.0f, 0.0f),
  m_timer(),
  m_state(IDLE)
{
  m_countMe = false;
  m_physic.set_velocity(0, 0);
  m_physic.enable_gravity(true);
  set_action("idle");
}

void
AngryStone::collision_solid(const CollisionHit& hit)
{
  if (m_frozen)
    BadGuy::collision_solid(hit);
  // TODO
#if 0
  if ((m_state == ATTACKING) &&
      (hit.normal.x == -m_attack_direction.x) && (hit.normal.y == m_attack_direction.y)) 
  {
    m_state = IDLE;
    sprite->set_action("idle");
    physic.set_velocity(0, 0);
    physic.enable_gravity(true);
    m_old_wall_direction.x = m_attack_direction.x;
    m_old_wall_direction.y = m_attack_direction.y;
  }
#endif
}

void
AngryStone::kill_fall()
{
  if (!m_frozen)
    return;
  BadGuy::kill_fall();
  // Prevents AngryStone from getting killed by other enemies or the player.
}

HitResponse
AngryStone::collision_badguy(BadGuy& badguy, const CollisionHit& )
{
  if (m_state == ATTACKING) 
  {
    badguy.kill_fall();
  }

  return FORCE_MOVE;
}

void
AngryStone::active_update(float dt_sec) 
{
  BadGuy::active_update(dt_sec);

  if (m_frozen)
    return;

  switch (m_state) 
  {
    case IDLE: 
    {
      auto player = get_nearest_player();
      if (player) 
      {
        auto badguy = this;
        const Vector& playerPos = player->get_pos();
        const Vector& badguyPos = badguy->get_pos();
        float dx = (playerPos.x - badguyPos.x);
        float dy = (playerPos.y - badguyPos.y);

        float playerHeight = player->get_bbox().get_height();
        float badguyHeight = badguy->get_bbox().get_height();

        float playerWidth = player->get_bbox().get_width();
        float badguyWidth = badguy->get_bbox().get_width();

        if ((dx > -playerWidth) && (dx < badguyWidth)) 
        {
          if (dy > 0) 
          {
            m_attack_direction.x = 0;
            m_attack_direction.y = 1;
          } else 
          {
            m_attack_direction.x = 0;
            m_attack_direction.y = -1;
          }
          if ((m_attack_direction.x != m_old_wall_direction.x) || (m_attack_direction.y != m_old_wall_direction.y)) 
          {
            set_action("charging");
            m_timer.start(CHARGE_TIME);
            m_state = CHARGING;
          }
        } else if ((dy > -playerHeight) && (dy < badguyHeight)) 
          {
          if (dx > 0) 
          {
            m_attack_direction.x = 1;
            m_attack_direction.y = 0;
          } else 
          {
            m_attack_direction.x = -1;
            m_attack_direction.y = 0;
          }
          if ((m_attack_direction.x != m_old_wall_direction.x) || (m_attack_direction.y != m_old_wall_direction.y)) 
          {
            set_action("charging");
            m_timer.start(CHARGE_TIME);
            m_state = CHARGING;
          }
        }
      }
    } break;

    case CHARGING: 
    {
      if (m_timer.check()) 
      {
        set_action("attacking");
        m_timer.start(ATTACK_TIME);
        m_state = ATTACKING;
        m_physic.enable_gravity(false);
        m_physic.set_velocity(CHARGE_SPEED * m_attack_direction.x,
                              CHARGE_SPEED * m_attack_direction.y);
        m_old_wall_direction.x = 0;
        m_old_wall_direction.y = 0;
      }
    } break;

    case ATTACKING: 
    {
      if (m_timer.check()) 
      {
        m_timer.start(RECOVER_TIME);
        m_state = RECOVERING;
        set_action("idle");
        m_physic.enable_gravity(true);
        m_physic.set_velocity(0, 0);
      }
    } break;

    case RECOVERING: 
    {
      if (m_timer.check()) 
      {
        m_state = IDLE;
        set_action("idle");
        m_physic.enable_gravity(true);
        m_physic.set_velocity(0, 0);
      }
    } break;
  }
}

void
AngryStone::freeze()
{
  BadGuy::freeze();
  m_state = IDLE;
  m_physic.enable_gravity(true);
}

void
AngryStone::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
}

bool
AngryStone::is_freezable() const
{
  return true;
}

bool
AngryStone::is_flammable() const
{
  return false;
}

std::vector<Direction>
AngryStone::get_allowed_directions() const
{
  return {};
}

/* EOF */
