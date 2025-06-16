//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#include "badguy/short_fuse.hpp"

#include "audio/sound_manager.hpp"
#include "object/bullet.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

ShortFuse::ShortFuse(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/short_fuse/short_fuse.sprite", "left", "right")
{
  walk_speed = 100;
  set_ledge_behavior(LedgeBehavior::SMART);

  SoundManager::current()->preload("sounds/firecracker.ogg");
}

void
ShortFuse::explode()
{
  if (!is_valid())
    return;

  if (m_frozen)
    BadGuy::kill_fall();
  else
  {
    Sector::get().add<Explosion>(get_bbox().get_middle(), 
      EXPLOSION_STRENGTH_NEAR, 8, true);

    run_dead_script();
    remove_me();
  }
}

bool
ShortFuse::collision_squished(MovingObject& obj)
{
  if (m_frozen)
    return WalkingBadguy::collision_squished(obj);

  if (!is_valid ())
    return true;

  auto player = dynamic_cast<Player*>(&obj);
  if (player)
    player->bounce(*this);

  explode ();

  return true;
}

HitResponse
ShortFuse::collision_player(Player& player, const CollisionHit&)
{
  if (!m_frozen)
  {
    player.bounce(*this);
    explode();
  }
  return FORCE_MOVE;
}

void
ShortFuse::freeze()
{
  m_col.m_bbox.move(Vector(0.f, -100.f));
  BadGuy::freeze();
}

void
ShortFuse::kill_fall()
{
  explode();
}

void
ShortFuse::ignite()
{
  if (m_frozen)
    unfreeze();
  kill_fall();
}

bool
ShortFuse::is_freezable() const
{
  return true;
}

/* EOF */
