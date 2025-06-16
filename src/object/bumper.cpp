//  Copyright (C) 2020 Daniel Ward <weluvgoatz@gmail.com>
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

#include "object/bumper.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "object/player.hpp"
#include "object/rock.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

namespace {
const std::string TRAMPOLINE_SOUND = "sounds/trampoline.wav";
const float BOUNCE_Y = -450.0f;
const float BOUNCE_X = 700.0f;
}

Bumper::Bumper(const ReaderMapping& reader) :
  StickyObject(reader, "images/objects/trampoline/bumper.sprite", LAYER_OBJECTS, COLGROUP_MOVING),
  m_physic(),
  m_dir(Direction::RIGHT),
  m_original_pos(get_pos())
{
  std::string dir_str;
  bool old_facing_left = false;

  reader.get("sticky", m_sticky, false);

  if (reader.get("direction", dir_str))
    m_dir = string_to_dir(dir_str);
  else if (reader.get("left", old_facing_left) && old_facing_left)
    m_dir = Direction::LEFT;

  set_action("default", m_dir);
  m_physic.enable_gravity(false);
}

ObjectSettings
Bumper::get_settings()
{
  ObjectSettings result = StickyObject::get_settings();

  result.add_direction(_("Direction"), &m_dir, { Direction::RIGHT, Direction::LEFT }, "direction");
  result.reorder({"sticky", "direction", "sprite", "x", "y"});
  return result;
}

void
Bumper::update(float dt_sec)
{
  if (m_sprite->animation_done())
    set_action("default", m_dir);

  // Pushing rocks, as well as dynamic with tilemap, platform, and fallblock.

  Rectf small_overlap_box = get_bbox().grown(1.f);

  for (auto& rock : Sector::get().get_objects_by_type<Rock>())
  {
    if (small_overlap_box.overlaps(rock.get_bbox()))
    {
      rock.get_physic().set_velocity((m_dir == Direction::LEFT ? -BOUNCE_X : BOUNCE_X) * 0.7f,
                                     BOUNCE_Y * 0.6f);
      SoundManager::current()->play(TRAMPOLINE_SOUND, get_pos());
      set_action("swinging", m_dir, 1);
      set_pos(m_original_pos);
    }
  }

  if (m_sticky)
  {
    if (m_sticking) {
      m_original_pos = get_pos();
    }
    StickyObject::update(dt_sec);
  }
  else {
    m_col.set_movement(m_physic.get_movement(dt_sec));
  }
}

HitResponse
Bumper::collision(MovingObject& other, const CollisionHit& hit)
{
  auto player = dynamic_cast<Player*> (&other);
  if (player)
  {
    player->get_physic().set_velocity(0.f, player->is_swimming() ? 0.f : BOUNCE_Y);
    player->sideways_push(m_dir == Direction::LEFT ? -BOUNCE_X : BOUNCE_X);
    bounce();
  }

  auto badguy = dynamic_cast<BadGuy*> (&other);
  if (badguy)
  {
    badguy->get_physic().set_velocity(((m_dir == Direction::LEFT) ? -400.f : 400.f), 0.f);
    SoundManager::current()->play(TRAMPOLINE_SOUND, get_pos());
    set_action("swinging", m_dir, 1);
  }

  auto bumper = dynamic_cast<Bumper*> (&other);
  if (bumper)
  {
    m_physic.set_velocity_y(0);
    return FORCE_MOVE;
  }
  return ABORT_MOVE;
}

Physic&
Bumper::get_physic()
{
  return m_physic;
}

void
Bumper::after_editor_set()
{
  MovingSprite::after_editor_set();

  set_action("default", m_dir);
}

void
Bumper::on_flip(float height)
{
  MovingSprite::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

void
Bumper::bounce()
{
  SoundManager::current()->play(TRAMPOLINE_SOUND, get_pos());
  set_action("swinging", m_dir, 1);
}

/* EOF */
