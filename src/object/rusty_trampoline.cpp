//  SuperTux - Rusty Trampoline
//  Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
//  Copyright (C) 2011 Jonas Kuemmerlin <rgcjonas@googlemail.com>
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

#include "object/rusty_trampoline.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/walking_badguy.hpp"
#include "control/controller.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "util/reader_mapping.hpp"

/* Trampoline will accelerate Tux to to VY_BOUNCE, if
 * he jumps on it to VY_TRIGGER. */
namespace {
const std::string BOUNCE_SOUND = "sounds/trampoline.wav";
const float VY_TRIGGER = -900; //negative, upwards
const float VY_BOUNCE = -500;
}

RustyTrampoline::RustyTrampoline(const ReaderMapping& mapping) :
  Rock(mapping, "images/objects/rusty-trampoline/rusty-trampoline.sprite"),
  portable(true), counter(3)
{
  SoundManager::current()->preload(BOUNCE_SOUND);

  mapping.get("counter", counter);
  mapping.get("portable", portable); //do we really need this?
}

void
RustyTrampoline::update(float dt_sec)
{
  if (m_sprite->animation_done()) {
    if (counter < 1) {
      remove_me();
    } else {
      set_action("default");
    }

  }

  Rock::update(dt_sec);
}

ObjectSettings
RustyTrampoline::get_settings()
{
  ObjectSettings result = Rock::get_settings();

  result.add_int(_("Counter"), &counter, "counter", 3);
  result.add_bool(_("Portable"), &portable, "portable", true);

  result.reorder({"counter", "x", "y"});

  return result;
}

HitResponse
RustyTrampoline::collision(MovingObject& other, const CollisionHit& hit)
{
  //Trampoline has to be on ground to work.
  if (m_on_ground) {
    auto player = dynamic_cast<Player*> (&other);
    //Trampoline works for player
    if (player) {
      float vy = player->get_physic().get_velocity_y();
      //player is falling down on trampoline
      if (hit.top && vy >= 0) {
        if (player->get_controller().hold(Control::JUMP)) {
          vy = VY_TRIGGER;
        } else {
          vy = VY_BOUNCE;
        }
        player->get_physic().set_velocity_y(vy);
        bounce();

        return FORCE_MOVE;
      }
    }
    auto walking_badguy = dynamic_cast<WalkingBadguy*> (&other);
    //Trampoline also works for WalkingBadguy
    if (walking_badguy) {
      float vy = walking_badguy->get_velocity_y();
      //walking_badguy is falling down on trampoline
      if (hit.top && vy >= 0) {
        vy = VY_BOUNCE;
        walking_badguy->set_velocity_y(vy);
        bounce();

        return FORCE_MOVE;
      }
    }
  }

  return Rock::collision(other, hit);
}

void
RustyTrampoline::bounce()
{
  SoundManager::current()->play(BOUNCE_SOUND, get_pos());
  counter--;
  if (counter > 0) {
    set_action("swinging", 1);
  } else {
    set_action("breaking", 1);
  }
}

void
RustyTrampoline::collision_solid(const CollisionHit& hit) {
  Rock::collision_solid(hit);
}

void
RustyTrampoline::grab(MovingObject& object, const Vector& pos, Direction dir) {
  Rock::grab(object, pos, dir);
}

void
RustyTrampoline::ungrab(MovingObject& object, Direction dir)
{
  Rock::ungrab(object, dir);
  set_action("breaking", 1);
  counter = 0; //remove in update()
}

bool
RustyTrampoline::is_portable() const
{
  return Rock::is_portable() && portable;
}

/* EOF */
