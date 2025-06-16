//  SuperTux badguy - walking flame that glows
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

#include "badguy/livefire.hpp"

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"

LiveFire::LiveFire(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/livefire/livefire.sprite", "left", "right"),
  death_sound("sounds/fall.wav"),
  state(STATE_WALKING)
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::SMART);
  m_lightsprite->set_color(Color(0.89f, 0.75f, 0.44f));
  m_glowing = true;
}

void
LiveFire::collision_solid(const CollisionHit& hit)
{
  if (state != STATE_WALKING) {
    BadGuy::collision_solid(hit);
    return;
  }
  WalkingBadguy::collision_solid(hit);
}

HitResponse
LiveFire::collision_badguy(BadGuy& badguy, const CollisionHit& hit)
{
  if (state != STATE_WALKING) {
    return BadGuy::collision_badguy(badguy, hit);
  }
  return WalkingBadguy::collision_badguy(badguy, hit);
}

void
LiveFire::active_update(float dt_sec) {

  // Remove when extinguish animation is done.
  if ((m_sprite->get_action() == "extinguish-left" || m_sprite->get_action() == "extinguish-right" )
    && m_sprite->animation_done()) remove_me();

  if (state == STATE_WALKING) {
    WalkingBadguy::active_update(dt_sec);
    return;
  }

  if (state == STATE_SLEEPING && m_col.get_group() == COLGROUP_MOVING) {

    auto player = get_nearest_player();
    if (player) {
      Rectf pb = player->get_bbox();

      bool inReach_left = (pb.get_right() >= m_col.m_bbox.get_right()-((m_dir == Direction::LEFT) ? 256 : 0));
      bool inReach_right = (pb.get_left() <= m_col.m_bbox.get_left()+((m_dir == Direction::RIGHT) ? 256 : 0));
      bool inReach_top = (pb.get_bottom() >= m_col.m_bbox.get_top());
      bool inReach_bottom = (pb.get_top() <= m_col.m_bbox.get_bottom());

      if (inReach_left && inReach_right && inReach_top && inReach_bottom) {
        // Wake up.
        set_action("waking", m_dir, 1);
        state = STATE_WAKING;
      }
    }
  }
  else if (state == STATE_WAKING) {
    if (m_sprite->animation_done()) {
      // Start walking.
      state = STATE_WALKING;
      WalkingBadguy::initialize();
    }
  }

  BadGuy::active_update(dt_sec);
}

void
LiveFire::freeze()
{
  // Attempting to freeze a flame causes it to go out.
  death_sound = "sounds/sizzle.ogg";
  kill_fall();
}

bool
LiveFire::is_freezable() const
{
  return true;
}

bool
LiveFire::is_flammable() const
{
  return false;
}

void
LiveFire::kill_fall()
{
  SoundManager::current()->play(death_sound, get_pos());
  // Emit a puff of smoke.
  Vector ppos = m_col.m_bbox.get_middle();
  Vector pspeed = Vector(0, -150);
  Vector paccel = Vector(0,0);
  Sector::get().add<SpriteParticle>("images/particles/smoke.sprite",
                                         "default", ppos, ANCHOR_MIDDLE,
                                         pspeed, paccel,
                                         LAYER_BACKGROUNDTILES+2);
  // Extinguish the flame.
  set_action("extinguish", m_dir, 1);
  m_physic.set_velocity_y(0);
  m_physic.set_acceleration_y(0);
  m_physic.enable_gravity(false);
  m_lightsprite->set_blend(Blend::ADD);
  m_lightsprite->set_color(Color(1.0f, 0.9f, 0.8f));
  set_group(COLGROUP_DISABLED);
  state = STATE_DEAD;

  // Start the dead-script.
  run_dead_script();
}

/* The following defines a sleeping version. */

LiveFireAsleep::LiveFireAsleep(const ReaderMapping& reader) :
  LiveFire(reader)
{
  state = STATE_SLEEPING;
}

void
LiveFireAsleep::after_editor_set()
{
  set_action("sleeping", m_dir);
}

void
LiveFireAsleep::draw(DrawingContext& context)
{
  if (Editor::is_active())
  {
    BadGuy::draw(context);
  }
  else
  {
    LiveFire::draw(context);
  }
}

void
LiveFireAsleep::initialize()
{
  m_physic.set_velocity_x(0);
  set_action("sleeping", m_dir);
}

/* The following defines a dormant version that remains inactive. */
LiveFireDormant::LiveFireDormant(const ReaderMapping& reader) :
  LiveFire(reader)
{
  walk_speed = 0;
  state = STATE_DORMANT;
}

void
LiveFireDormant::after_editor_set()
{
  set_action("sleeping", m_dir);
}

void
LiveFireDormant::draw(DrawingContext& context)
{
  if (Editor::is_active())
  {
    BadGuy::draw(context);
  }
  else
  {
    LiveFire::draw(context);
  }
}

void
LiveFireDormant::initialize()
{
  m_physic.set_velocity_x(0);
  set_action("sleeping", m_dir);
}

/* EOF */
