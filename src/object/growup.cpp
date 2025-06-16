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

#include "object/growup.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "math/util.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"

GrowUp::GrowUp(const Vector& pos, Direction direction, const std::string& custom_sprite) :
  MovingSprite(pos, custom_sprite.empty() ? "images/powerups/egg/egg.sprite" : custom_sprite, LAYER_OBJECTS, COLGROUP_MOVING),
  m_physic(),
  m_custom_sprite(!custom_sprite.empty()),
  m_shadesprite(SpriteManager::current()->create("images/powerups/egg/egg.sprite")),
  m_lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-small.sprite"))
{
  m_physic.enable_gravity(true);
  m_physic.set_velocity_x((direction == Direction::LEFT) ? -100.0f : 100.0f);
  SoundManager::current()->preload("sounds/grow.ogg");
  // Set the shadow action for the egg sprite, so it remains in place as the egg rolls.
  m_shadesprite->set_action("shadow");
  // Configure the light sprite for the glow effect.
  m_lightsprite->set_blend(Blend::ADD);
  m_lightsprite->set_color(Color(0.2f, 0.2f, 0.0f));
}

void
GrowUp::update(float dt_sec)
{
  if (!m_custom_sprite && m_physic.get_velocity_x() != 0)
    m_sprite->set_angle(get_pos().x * 360.0f / (32.0f * math::PI));

  m_col.set_movement(m_physic.get_movement(dt_sec));
}

void
GrowUp::draw(DrawingContext& context)
{
  MovingSprite::draw(context);

  if (m_custom_sprite)
    return;

  m_shadesprite->draw(context.color(), get_pos(), m_layer);
  m_lightsprite->draw(context.light(), get_bbox().get_middle(), 0);
}

void
GrowUp::collision_solid(const CollisionHit& hit)
{
  if (hit.top)
    m_physic.set_velocity_y(0);

  if (hit.bottom && m_physic.get_velocity_y() > 0)
    m_physic.set_velocity_y(0);

  if (hit.slope_normal.x == 0.0f &&
      ((hit.left && m_physic.get_velocity_x() < 0.0f) ||
      (hit.right && m_physic.get_velocity_x() > 0.0f))) {
    m_physic.set_velocity_x(-m_physic.get_velocity_x());
    m_physic.set_acceleration_x(-m_physic.get_acceleration_x());
  }
}

HitResponse
GrowUp::collision(MovingObject& other, const CollisionHit& hit )
{
  auto player = dynamic_cast<Player*>(&other);
  if (player != nullptr) {
    if (!player->add_bonus(BONUS_GROWUP, true)) {
      // Tux can't grow right now.
      collision_solid( hit );
      return ABORT_MOVE;
    }

    SoundManager::current()->play("sounds/grow.ogg", get_pos());
    remove_me();

    return ABORT_MOVE;
  }

  return FORCE_MOVE;
}

void
GrowUp::do_jump()
{
  m_physic.set_velocity_y(-300);
}

/* EOF */
