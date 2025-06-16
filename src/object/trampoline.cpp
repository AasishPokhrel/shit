//  SuperTux - Trampoline
//  Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
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

#include "object/trampoline.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "control/controller.hpp"
#include "object/player.hpp"
#include "object/coin.hpp"
#include "util/reader_mapping.hpp"

/* Trampoline will accelerate Tux to to VY_INITIAL, if
 * he jumps on it to VY_MIN. */
namespace {
  const std::string TRAMPOLINE_SOUND = "sounds/trampoline.wav";
  const float VY_MIN = -900; //negative, upwards
  const float VY_INITIAL = -500;
} // namespace

Trampoline::Trampoline(const ReaderMapping& mapping) :
  Rock(mapping, "images/objects/trampoline/trampoline.sprite")
{
  // Older levels use the "portable" property to determine the type.
  bool portable = true;
  mapping.get("portable", portable);
  if (!portable)
  {
    m_type = STATIONARY;
    on_type_change(TypeChange::INITIAL);
  }
  else
  {
    parse_type(mapping);
  }

  SoundManager::current()->preload(TRAMPOLINE_SOUND);
}

Trampoline::Trampoline(const Vector& pos, int type) :
  Rock(pos, "images/objects/trampoline/trampoline.sprite")
{
  m_type = type;
  on_type_change(TypeChange::INITIAL);

  SoundManager::current()->preload(TRAMPOLINE_SOUND);
}

GameObjectTypes
Trampoline::get_types() const
{
  return {
    { "portable", _("Portable") },
    { "stationary", _("Stationary") }
  };
}

std::string
Trampoline::get_default_sprite_name() const
{
  switch (m_type)
  {
    case STATIONARY:
      return "images/objects/trampoline/trampoline_fix.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
Trampoline::update(float dt_sec)
{
  if (m_sprite->animation_done()) {
    set_action("default");
  }

  Rock::update(dt_sec);
}

HitResponse
Trampoline::collision(MovingObject& other, const CollisionHit& hit)
{
  auto heavy_coin = dynamic_cast<HeavyCoin*> (&other);
  if (heavy_coin) {
    return ABORT_MOVE;
  }
  auto player = dynamic_cast<Player*> (&other);
  //Trampoline works for player
  if (player && !is_grabbed())
  {
    player->override_velocity();
    if (player->m_does_buttjump)
      player->m_does_buttjump = false;
    float vy = player->get_physic().get_velocity_y();
    //player is falling down on trampoline
    if (hit.top && vy >= 0)
    {
      if (!(player->get_status().bonus[player->get_id()] == BONUS_AIR))
      {
        if (player->get_controller().hold(Control::JUMP))
          vy = VY_MIN;
        else if (player->is_big() && player->get_controller().hold(Control::DOWN))
          vy = VY_MIN + 100;
        else
          vy = VY_INITIAL;
      }
      else
      {
        if (player->get_controller().hold(Control::JUMP))
          vy = VY_MIN - 80;
        else if (player->get_controller().hold(Control::DOWN))
          vy = VY_MIN - 70;
        else
          vy = VY_INITIAL - 40;
      }
      player->get_physic().set_velocity_y(vy);
      bounce();
      return FORCE_MOVE;
    }
  }
  auto badguy = dynamic_cast<BadGuy*> (&other);
  //Trampoline also works for Badguy
  if (badguy) {
    float vy = badguy->get_physic().get_velocity_y();
    //badguy is falling down on trampoline
    if (hit.top && vy >= 0) {
      vy = VY_INITIAL;
      badguy->get_physic().set_velocity_y(vy);
      SoundManager::current()->play(TRAMPOLINE_SOUND, get_pos());
      set_action("swinging", 1);
      return FORCE_MOVE;
    }
  }

  return Rock::collision(other, hit);
}

void
Trampoline::grab(MovingObject& object, const Vector& pos, Direction dir)
{
  m_sprite->set_animation_loops(0);
  Rock::grab(object, pos, dir);
}

bool
Trampoline::is_portable() const
{
  return Rock::is_portable() && m_type == PORTABLE;
}

void
Trampoline::bounce()
{
  SoundManager::current()->play(TRAMPOLINE_SOUND, get_pos());
  m_sprite->set_action("swinging", 1);
}

/* EOF */
