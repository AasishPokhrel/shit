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

#include "badguy/sspiky.hpp"

#include "object/player.hpp"
#include "sprite/sprite.hpp"

SSpiky::SSpiky(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/spiky/sleepingspiky.sprite", "left", "right"), state(SSPIKY_SLEEPING)
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::NORMAL);
}

void
SSpiky::initialize()
{
  state = SSPIKY_SLEEPING;
  m_physic.set_velocity_x(0);
  set_action("sleeping", m_dir);
}

void
SSpiky::collision_solid(const CollisionHit& hit)
{
  if (state != SSPIKY_WALKING) {
    BadGuy::collision_solid(hit);
    return;
  }
  WalkingBadguy::collision_solid(hit);
}

HitResponse
SSpiky::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (state != SSPIKY_WALKING) {
    return BadGuy::collision_badguy(badguy, hit);
  }
  return WalkingBadguy::collision_badguy(badguy, hit);
}

void
SSpiky::active_update(float dt_sec) {

  if (state == SSPIKY_WALKING) {
    WalkingBadguy::active_update(dt_sec);
    return;
  }

  if (state == SSPIKY_SLEEPING) {

    Player* player = get_nearest_player();
    if (player) {
      Rectf pb = player->get_bbox();

      bool inReach_left = (pb.get_right() >= m_col.m_bbox.get_right()-((m_dir == Direction::LEFT) ? 256 : 0));
      bool inReach_right = (pb.get_left() <= m_col.m_bbox.get_left()+((m_dir == Direction::RIGHT) ? 256 : 0));
      bool inReach_top = (pb.get_bottom() >= m_col.m_bbox.get_top());
      bool inReach_bottom = (pb.get_top() <= m_col.m_bbox.get_bottom());

      if (inReach_left && inReach_right && inReach_top && inReach_bottom) {
        // Wake up.
        set_action("waking", m_dir, 1);
        state = SSPIKY_WAKING;
      }
    }

    BadGuy::active_update(dt_sec);
  }

  if (state == SSPIKY_WAKING) {
    if (m_sprite->animation_done()) {
      // Start walking.
      state = SSPIKY_WALKING;
      WalkingBadguy::initialize();
    }

    BadGuy::active_update(dt_sec);
  }
}

void
SSpiky::freeze()
{
  WalkingBadguy::freeze();
  state = SSPIKY_WALKING; // If we get hit while sleeping, wake up.
}

bool
SSpiky::is_freezable() const
{
  return true;
}

bool
SSpiky::is_flammable() const
{
  return state != SSPIKY_SLEEPING;
}

void
SSpiky::after_editor_set()
{
  WalkingBadguy::after_editor_set();
  if (m_start_dir == Direction::AUTO)
    set_action("sleeping-left");
}

/* EOF */
