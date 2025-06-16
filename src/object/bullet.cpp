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

#include "object/bullet.hpp"

#include "math/random.hpp"
#include "object/camera.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/direction.hpp"
#include "supertux/sector.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

Bullet::Bullet(const Vector& pos, const Vector& xm, Direction dir, BonusType type_, Player& player) :
  m_player(player),
  physic(),
  life_count(3),
  sprite(),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-small.sprite")),
  type(type_)
{
  physic.set_velocity(xm);

  switch (type) {
    case BONUS_FIRE:
      sprite = SpriteManager::current()->create("images/objects/bullets/firebullet.sprite");
      lightsprite->set_blend(Blend::ADD);
      lightsprite->set_color(Color(0.3f, 0.1f, 0.0f));
      break;

    case BONUS_ICE:
      sprite = SpriteManager::current()->create("images/objects/bullets/icebullet.sprite");
      break;

    default:
      log_warning << "Bullet::Bullet called with unknown BonusType" << std::endl;
      life_count = 10;
      sprite = SpriteManager::current()->create("images/objects/bullets/firebullet.sprite");
      break;
  }

  m_col.m_bbox.set_pos(pos);
  m_col.m_bbox.set_size(sprite->get_current_hitbox_width(), sprite->get_current_hitbox_height());
}

void
Bullet::update(float dt_sec)
{
  // Cause fireball color to flicker randomly.
  if (graphicsRandom.rand(5) != 0) {
    lightsprite->set_color(Color(0.3f + graphicsRandom.randf(10) / 100.0f,
                                 0.1f + graphicsRandom.randf(20.0f) / 100.0f,
                                 graphicsRandom.randf(10.0f) / 100.0f));
  } else
    lightsprite->set_color(Color(0.3f, 0.1f, 0.0f));

  if (life_count <= 0)
  {
    remove_me();
    return;
  }

  float scroll_x = Sector::get().get_camera().get_translation().x;
  float scroll_y = Sector::get().get_camera().get_translation().y;
  float scale = Sector::get().get_camera().get_current_scale();
  if (get_pos().x < scroll_x ||
      get_pos().x > scroll_x + static_cast<float>(SCREEN_WIDTH) / scale ||
      //     get_pos().y < scroll_y ||
      get_pos().y > scroll_y + static_cast<float>(SCREEN_HEIGHT) / scale) {
    remove_me();
    return;
  }

  bool in_water = !Sector::get().is_free_of_tiles(get_bbox(), true, Tile::WATER);
  physic.set_gravity_modifier(in_water ? 0.3f : 1.f);
  m_col.set_movement(physic.get_movement(dt_sec) * (in_water ? 0.5f : 1.f));
}

void
Bullet::draw(DrawingContext& context)
{
  sprite->draw(context.color(), get_pos(), LAYER_OBJECTS);
  if (type == BONUS_FIRE){
    lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), 0);
  }
}

void
Bullet::collision_solid(const CollisionHit& hit)
{
  if (hit.top || hit.bottom) {
    physic.set_velocity_y(-physic.get_velocity_y());
    life_count--;
  } else if (hit.left || hit.right) {
    if (type == BONUS_ICE) {
      physic.set_velocity_x(-physic.get_velocity_x());
      life_count--;
    } else
      remove_me();
  }
}

void
Bullet::ricochet(GameObject& , const CollisionHit& hit)
{
  collision_solid(hit);
}

HitResponse
Bullet::collision(MovingObject& , const CollisionHit& )
{
  return FORCE_MOVE;
}

/* EOF */
