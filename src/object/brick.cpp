//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "object/brick.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "badguy/crusher.hpp"
#include "object/bouncy_coin.hpp"
#include "object/camera.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "supertux/constants.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

Brick::Brick(const Vector& pos, int data, const std::string& sprite_name) :
  Block(pos, sprite_name),
  m_breakable(false),
  m_coin_counter(0)
{
  if (data == 1) {
    m_coin_counter = 5;
  } else {
    m_breakable = true;
  }
}

Brick::Brick(const ReaderMapping& mapping, const std::string& sprite_name) :
  Block(mapping, sprite_name),
  m_breakable(),
  m_coin_counter(0)
{
  parse_type(mapping);
  mapping.get("breakable", m_breakable, true);
  if (!m_breakable) {
    m_coin_counter = 5;
  }
}

GameObjectTypes
Brick::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "retro", _("Retro") }
  };
}

std::string
Brick::get_default_sprite_name() const
{
  switch (m_type)
  {
    case RETRO:
      return "images/objects/bonus_block/retro_brick.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
Brick::hit(Player& player)
{
  if (m_sprite->get_action() == "empty")
    return;

  try_break(&player);
}

HitResponse
Brick::collision(MovingObject& other, const CollisionHit& hit)
{
  auto player = dynamic_cast<Player*> (&other);
  if (player && player->m_does_buttjump) try_break(player);

  auto badguy = dynamic_cast<BadGuy*> (&other);
  if (badguy) {
    // Hit contains no information for collisions with blocks.
    // Badguy's bottom has to be below the top of the brick
    // SHIFT_DELTA is required to slide over one tile gaps.
    if ( badguy->can_break() && ( badguy->get_bbox().get_bottom() > m_col.m_bbox.get_top() + SHIFT_DELTA ) ) {
      try_break(nullptr);
    }
  }
  auto portable = dynamic_cast<Portable*> (&other);
  if (portable && !badguy) {
    if (other.get_bbox().get_top() > m_col.m_bbox.get_bottom() - SHIFT_DELTA) {
      try_break(nullptr);
    }
  }

  auto explosion = dynamic_cast<Explosion*> (&other);
  if (explosion && explosion->hurts()) {
    try_break(nullptr);
  }

  auto crusher = dynamic_cast<Crusher*> (&other);
  if (crusher && crusher->get_state() == Crusher::CRUSHING && m_coin_counter == 0)
    try_break(nullptr);

  return Block::collision(other, hit);
}

void
Brick::try_break(Player* player, bool slider)
{
  if (m_sprite->get_action() == "empty")
    return;

  // Takes too long for sliding tux to barrel through crates and ends up stopping him otherwise.
  if (slider && m_breakable && m_coin_counter <= 0)
    break_me();

  SoundManager::current()->play("sounds/brick.wav", get_pos());
  if (m_coin_counter > 0 ) {
    Sector::get().add<BouncyCoin>(get_pos(), true);
    m_coin_counter--;
    Player& player_one = *Sector::get().get_players()[0];
    player_one.get_status().add_coins(1);
    if (m_coin_counter == 0)
      set_action("empty");
    start_bounce(player);
  } else if (m_breakable) {
    if (player) {
      if (player->is_big()) {
        start_break(player);
      } else {
        start_bounce(player);
      }
      return;
    }
    break_me();
  }
}

void
Brick::break_for_crusher(Crusher* crusher)
{
  // FIXME: This function shouldn't even exist
   float shake_vel_x = crusher->is_sideways() ? crusher->get_physic().get_velocity_x() >= 0.f ? 6.f : -6.f : 0.f;
   float shake_vel_y = crusher->is_sideways() ? 0.f : 6.f;
   Sector::get().get_camera().shake(0.1f, shake_vel_x, shake_vel_y);
   try_break(nullptr);
   start_break(crusher);
}

ObjectSettings
Brick::get_settings()
{
  ObjectSettings result = Block::get_settings();
  result.add_bool(_("Breakable"), &m_breakable, "breakable");
  return result;
}

HeavyBrick::HeavyBrick(const Vector& pos, int data, const std::string& spriteName) :
  Brick(pos, data, spriteName)
{
}

HeavyBrick::HeavyBrick(const ReaderMapping& mapping) :
  Brick(mapping, "images/objects/bonus_block/heavy-brick.sprite")
{
}

HitResponse
HeavyBrick::collision(MovingObject& other, const CollisionHit& hit)
{
  auto player = dynamic_cast<Player*>(&other);
  if (player && player->m_does_buttjump) ricochet(&other);

  auto crusher = dynamic_cast<Crusher*>(&other);
  if (crusher && crusher->get_state() == Crusher::CRUSHING)
  {
    if (crusher->is_big())
      try_break(nullptr);
    else
      ricochet(&other);
  }

  auto badguy = dynamic_cast<BadGuy*>(&other);
  if (badguy && badguy->can_break() && (badguy->get_bbox().get_bottom() > m_col.m_bbox.get_top() + SHIFT_DELTA ))
    ricochet(&other);

  auto portable = dynamic_cast<Portable*>(&other);
  if (portable)
  {
    if (other.get_bbox().get_top() > m_col.m_bbox.get_bottom() - SHIFT_DELTA)
      ricochet(&other);
  }

  // Skip Brick::collision
  // TODO: Make the Brick class an absract class and have the normal brick and
  //       heavy brick both inherit that class?
  return Block::collision(other, hit);
}

void
HeavyBrick::ricochet(MovingObject* collider)
{
  SoundManager::current()->play("sounds/metal_hit.ogg", get_pos());
  start_bounce(collider);
}

void
HeavyBrick::hit(Player& player)
{
  ricochet(&player);
}

/* EOF */
