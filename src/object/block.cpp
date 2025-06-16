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

#include <physfs.h>

#include "object/block.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "badguy/mrbomb.hpp"
#include "math/random.hpp"
#include "object/coin.hpp"
#include "object/growup.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "supertux/constants.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

static const float BOUNCY_BRICK_MAX_OFFSET = 8;
static const float BOUNCY_BRICK_SPEED = 90;
static const float BUMP_ROTATION_ANGLE = 10;

Block::Block(const Vector& pos, const std::string& sprite_file) :
  MovingSprite(pos, sprite_file, LAYER_OBJECTS + 1),
  m_bouncing(false),
  m_breaking(false),
  m_bounce_dir(0),
  m_bounce_offset(0),
  m_original_y(-1)
{
  m_col.m_bbox.set_size(32, 32.1f);
  set_group(COLGROUP_STATIC);
  SoundManager::current()->preload("sounds/upgrade.wav");
  SoundManager::current()->preload("sounds/brick.wav");
}

Block::Block(const ReaderMapping& mapping, const std::string& sprite_file) :
  MovingSprite(mapping, sprite_file, LAYER_OBJECTS + 1),
  m_bouncing(false),
  m_breaking(false),
  m_bounce_dir(0),
  m_bounce_offset(0),
  m_original_y(-1)
{
  m_col.m_bbox.set_size(32, 32.1f);
  set_group(COLGROUP_STATIC);
  SoundManager::current()->preload("sounds/upgrade.wav");
  SoundManager::current()->preload("sounds/brick.wav");
}

HitResponse
Block::collision(MovingObject& other, const CollisionHit& )
{
  auto player = dynamic_cast<Player*> (&other);
  if (player)
  {
    bool in_line_with_player = ((player->get_physic().get_velocity_x() > 0.f &&
      player->get_bbox().get_right() < get_bbox().get_left()) ||
      (player->get_physic().get_velocity_x() < 0.f &&
      player->get_bbox().get_left() > get_bbox().get_right()));

    if(player->is_swimboosting() || ((player->is_sliding() || player->is_stone()) && in_line_with_player))
    {
      hit(*player);
    }
    else if (!player->is_water_jumping() && !player->is_swimming())
    {
      bool x_coordinates_intersect =
        player->get_bbox().get_right() >= m_col.m_bbox.get_left() &&
        player->get_bbox().get_left() <= m_col.m_bbox.get_right();
      if (player->get_bbox().get_top() > m_col.m_bbox.get_bottom() - SHIFT_DELTA &&
          x_coordinates_intersect)
      {
        hit(*player);
      }
    }
  }

  // Only interact with other objects if:
  //   1) we are bouncing
  //   2) the object is not portable (either never or not currently)
  //   3) the object is being hit from below (baguys don't get killed for activating boxes).
  auto badguy = dynamic_cast<BadGuy*> (&other);
  auto portable = dynamic_cast<Portable*> (&other);
  bool is_portable = ((portable != nullptr) && portable->is_portable());
  bool hit_mo_from_below = other.get_bbox().get_bottom() < m_col.m_bbox.get_top() + SHIFT_DELTA;
  if (m_bouncing && (!is_portable || badguy) && hit_mo_from_below) {

    // Badguys get killed.
    if (badguy) {
      badguy->kill_fall();
    }

    // Coins get collected.
    auto coin = dynamic_cast<Coin*> (&other);
    if (coin) {
      coin->collect();
    }

    // Eggs get jumped.
    auto growup = dynamic_cast<GrowUp*> (&other);
    if (growup) {
      growup->do_jump();
    }

  }

  return FORCE_MOVE;
}


void
Block::update(float dt_sec)
{
  if (!m_bouncing)
    return;

  float offset = m_original_y - get_pos().y;
  if (offset > BOUNCY_BRICK_MAX_OFFSET) {
    m_bounce_dir = BOUNCY_BRICK_SPEED;
    m_col.set_movement(Vector(0, m_bounce_dir * dt_sec));
    if (m_breaking){
      break_me();
    }
  } else if (offset < BOUNCY_BRICK_SPEED * dt_sec && m_bounce_dir > 0) {
    m_col.set_movement(Vector(0, offset));
    m_bounce_dir = 0;
    m_bouncing = false;
    m_sprite->set_angle(0);
  } else {
    m_col.set_movement(Vector(0, m_bounce_dir * dt_sec));
  }
}

void
Block::start_bounce(MovingObject* hitter)
{
  if (m_original_y == -1){
    m_original_y = m_col.m_bbox.get_top();
  }
  m_bouncing = true;
  m_bounce_dir = -BOUNCY_BRICK_SPEED;
  m_bounce_offset = 0;

  if (!hitter) return;

  float center_of_hitter = hitter->get_bbox().get_middle().x;
  float offset = (m_col.m_bbox.get_middle().x - center_of_hitter)*2 / m_col.m_bbox.get_width();

  // Without this, hitting a multi-coin bonus block from the side (e. g. with
  // an ice block or a snail) would turn the block 90 degrees.
  if (offset > 2 || offset < -2)
    offset = 0;

  m_sprite->set_angle(BUMP_ROTATION_ANGLE*offset);
}

void
Block::start_break(MovingObject* hitter)
{
  start_bounce(hitter);
  m_breaking = true;
}

void
Block::break_me()
{
  const auto gravity = Sector::get().get_gravity() * 100;
  Vector pos = get_pos() + Vector(16.0f, 16.0f);

  for (const char* action : {"piece1", "piece2", "piece3", "piece4", "piece5", "piece6"})
  {
    Vector velocity(graphicsRandom.randf(-100, 100),
                    graphicsRandom.randf(-400, -300));
    Sector::get().add<SpriteParticle>(m_sprite->clone(), action,
                                pos, ANCHOR_MIDDLE,
                                velocity, Vector(0, gravity),
                                m_layer);
  }

  remove_me();
}

void
Block::on_flip(float height)
{
  MovingObject::on_flip(height);
  if (m_original_y != -1) m_original_y = height - m_original_y - get_bbox().get_height();
  FlipLevelTransformer::transform_flip(m_flip);
}

/* EOF */
