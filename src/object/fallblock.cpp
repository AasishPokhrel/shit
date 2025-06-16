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

#include "object/fallblock.hpp"

#include "audio/sound_manager.hpp"
#include "object/bumper.hpp"
#include "object/player.hpp"
#include "object/camera.hpp"
#include "sprite/sprite.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"
#include "math/random.hpp"
#include "util/reader_mapping.hpp"

FallBlock::FallBlock(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/fallblock/cave-4x4.sprite", LAYER_OBJECTS, COLGROUP_STATIC),
  m_state(IDLE),
  m_physic(),
  m_timer()
{
  SoundManager::current()->preload("sounds/cracking.wav");
  SoundManager::current()->preload("sounds/thud.ogg");
  m_physic.enable_gravity(false);
}

void
FallBlock::update(float dt_sec)
{
  switch (m_state)
  {
    case IDLE:
      set_group(COLGROUP_STATIC);
      if (found_victim_down())
      {
        m_state = SHAKE;
        SoundManager::current()->play("sounds/cracking.wav", get_pos());
        m_timer.start(0.5f);
      }
      break;
    case SHAKE:
      if (m_timer.check())
      {
        m_state = FALL;
        m_physic.reset();
        m_physic.enable_gravity(true);
      }
      break;
    case FALL:
    case LAND:
      m_col.set_movement(m_physic.get_movement (dt_sec));
      set_group(COLGROUP_MOVING_STATIC);
      break;
  }
}

HitResponse
FallBlock::collision(MovingObject& other, const CollisionHit& hit)
{
  auto fallblock = dynamic_cast<FallBlock*> (&other);
  if (fallblock && hit.bottom && (m_state == FALL || m_state == LAND))
  {
    m_physic.set_velocity_y(0.0f);
    return CONTINUE;
  }

  auto player = dynamic_cast<Player*>(&other);
  if (m_state == IDLE && player && player->get_bbox().get_bottom() < m_col.m_bbox.get_top())
  {
    m_state = SHAKE;
    SoundManager::current()->play("sounds/cracking.wav", get_pos());
    m_timer.start(0.5f);
  }
  return FORCE_MOVE;
}

void
FallBlock::collision_solid(const CollisionHit& hit)
{
  if (hit.top || hit.bottom || hit.crush)
  {
    m_physic.set_velocity(0.0f, 0.0f);
  }

  if (m_state == FALL && hit.bottom)
  {
    Sector::get().get_camera().shake(0.125f, 0.0f, 10.0f);
    SoundManager::current()->play("sounds/thud.ogg", get_pos());
    m_state = LAND;
  }
}

void
FallBlock::draw(DrawingContext& context)
{
  Vector pos = get_pos();
  // shaking
  if (m_state == SHAKE)
  {
    pos.x += static_cast<float>(graphicsRandom.rand(-8, 8));
    pos.y += static_cast<float>(graphicsRandom.rand(-5, 5));
  }
  m_sprite->draw(context.color(), pos, m_layer, m_flip);
}

bool
FallBlock::found_victim_down() const
{
  if (auto* player = Sector::get().get_nearest_player(m_col.m_bbox))
  {
    const Rectf& player_bbox = player->get_bbox();
    Rectf crush_area_down = Rectf(m_col.m_bbox.get_left()+1, m_col.m_bbox.get_bottom(),
                                  m_col.m_bbox.get_right()-1, std::max(m_col.m_bbox.get_bottom(),player_bbox.get_top()-1));
    if ((player_bbox.get_top() >= m_col.m_bbox.get_bottom())
        && (player_bbox.get_right() > (m_col.m_bbox.get_left() - 4))
        && (player_bbox.get_left() < (m_col.m_bbox.get_right() + 4))
        && (Sector::get().is_free_of_statics(crush_area_down, this, false)))
    {
      return true;
    }
  }
  return false;
}

void
FallBlock::on_flip(float height)
{
  MovingSprite::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

/* EOF */
