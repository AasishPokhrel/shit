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

#include "object/star.hpp"

#include "math/random.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

static const float INITIALJUMP = -400;
static const float STAR_SPEED = 150;
static const float JUMPSTAR_SPEED = -300;

Star::Star(const Vector& pos, Direction direction, const std::string& custom_sprite) :
  MovingSprite(pos, custom_sprite.empty() ? "images/powerups/star/star.sprite" : custom_sprite, LAYER_OBJECTS, COLGROUP_MOVING),
  physic(),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-small.sprite"))
{
  physic.set_velocity((direction == Direction::LEFT) ? -STAR_SPEED : STAR_SPEED, INITIALJUMP);
  //set light for glow effect
  lightsprite->set_blend(Blend::ADD);
  lightsprite->set_color(Color(0.4f, 0.4f, 0.4f));
}

void
Star::update(float dt_sec)
{
  m_col.set_movement(physic.get_movement(dt_sec));

  // when near Tux, spawn particles
  if (auto* player = Sector::get().get_nearest_player (m_col.m_bbox)) {
    float disp_x = player->get_bbox().get_left() - m_col.m_bbox.get_left();
    float disp_y = player->get_bbox().get_top() - m_col.m_bbox.get_top();
    if (disp_x*disp_x + disp_y*disp_y <= 256*256)
    {
      if (graphicsRandom.rand(0, 2) == 0) {
        float px = graphicsRandom.randf(m_col.m_bbox.get_left()+0, m_col.m_bbox.get_right()-0);
        float py = graphicsRandom.randf(m_col.m_bbox.get_top()+0, m_col.m_bbox.get_bottom()-0);
        Vector ppos = Vector(px, py);
        Vector pspeed = Vector(0, 0);
        Vector paccel = Vector(0, 0);
        Sector::get().add<SpriteParticle>(
          "images/particles/sparkle.sprite",
          // draw bright sparkles when very close to Tux, dark sparkles when slightly further
          (disp_x*disp_x + disp_y*disp_y <= 128*128) ?
          // make every other a longer sparkle to make trail a bit fuzzy
          (size_t(g_game_time*20)%2) ? "small" : "medium" : "dark",
          ppos, ANCHOR_MIDDLE, pspeed, paccel, LAYER_OBJECTS+1+5);
      }
    }
  }
}

void
Star::draw(DrawingContext& context)
{
  MovingSprite::draw(context);
  lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), 0);
}

void
Star::collision_solid(const CollisionHit& hit)
{
  if (hit.bottom) {
    physic.set_velocity_y(JUMPSTAR_SPEED);
  } else if (hit.top) {
    physic.set_velocity_y(0);
  } else if (hit.left || hit.right) {
    physic.set_velocity_x(-physic.get_velocity_x());
  }
}

HitResponse
Star::collision(MovingObject& other, const CollisionHit& )
{
  auto player = dynamic_cast<Player*> (&other);
  if (player) {
    player->make_invincible();
    remove_me();
    return ABORT_MOVE;
  }

  return FORCE_MOVE;
}

/* EOF */
