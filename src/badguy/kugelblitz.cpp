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

#include "badguy/kugelblitz.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "math/random.hpp"
#include "object/electrifier.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

namespace {

const float LIFETIME = 5.0f;
const float MOVETIME = 0.75f;
const int BASE_SPEED = 200;
const int RAND_SPEED = 150;

} // namespace

Kugelblitz::Kugelblitz(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/kugelblitz/kugelblitz.sprite"),
  pos_groundhit(0.0f, 0.0f),
  groundhit_pos_set(false),
  dying(),
  movement_timer(),
  lifetime(),
  direction(),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light.sprite"))
{
  m_start_position.x = m_col.m_bbox.get_left();
  set_action("falling");
  m_physic.enable_gravity(false);
  m_countMe = false;

  lightsprite->set_blend(Blend::ADD);
  lightsprite->set_color(Color(0.2f, 0.1f, 0.0f));

  SoundManager::current()->preload("sounds/lightning.wav");
}

void
Kugelblitz::initialize()
{
  m_physic.set_velocity(-20, 300); // Fall a little to the left.
  direction = 1;
  dying = false;
}

void
Kugelblitz::collision_solid(const CollisionHit& chit)
{
  hit(chit);
}

HitResponse
Kugelblitz::collision_player(Player& player, const CollisionHit& )
{
  if (player.is_invincible()) {
    explode();
    return ABORT_MOVE;
  }
  // Did the collision occur from above?
  if (player.get_movement().y - get_movement().y > 0 && player.get_bbox().get_bottom() <
     (m_col.m_bbox.get_top() + m_col.m_bbox.get_bottom()) / 2) {
    // If not, and if it's possible for the player to squish us, then this collision will hurt.
    if (!collision_squished(player))
      player.kill(false);
    explode();
    return FORCE_MOVE;
  }
  player.kill(false);
  explode();
  return FORCE_MOVE;
}

HitResponse
Kugelblitz::collision_badguy(BadGuy& other , const CollisionHit& chit)
{
  // Should the Kugelblitz explode as well? The concern is that
  // two Kugelblitzes would cancel each other out on contact.
  other.kill_fall();
  return hit(chit);
}

HitResponse
Kugelblitz::hit(const CollisionHit& hit_)
{
  // Did the collision occur with the floor?
  if (hit_.bottom) {
    if (!groundhit_pos_set)
    {
      pos_groundhit = get_pos();
      groundhit_pos_set = true;
    }
    set_action("flying");
    m_physic.set_velocity_y(0);
    // Set random initial speed and direction.
    direction = gameRandom.rand(2)? 1: -1;
    int speed = (BASE_SPEED + (gameRandom.rand(RAND_SPEED))) * direction;
    m_physic.set_velocity_x(static_cast<float>(speed));
    movement_timer.start(MOVETIME);
    lifetime.start(LIFETIME);

  }

  return CONTINUE;
}

void
Kugelblitz::active_update(float dt_sec)
{
  if (lifetime.check()) {
    explode();
  }
  else {
    if (groundhit_pos_set) {
      if (movement_timer.check()) {
        if (direction == 1) direction = -1; else direction = 1;
        int speed = (BASE_SPEED + (gameRandom.rand(RAND_SPEED))) * direction;
        m_physic.set_velocity_x(static_cast<float>(speed));
        movement_timer.start(MOVETIME);
      }
    }

    if (is_in_water()) {
      Sector::get().add<Electrifier>(Electrifier::TileChangeMap({ {75, 1421}, {76, 1422} }), 1.5f);
      explode();
    }
  }
  BadGuy::active_update(dt_sec);
}

void
Kugelblitz::draw(DrawingContext& context)
{
  m_sprite->draw(context.color(), get_pos(), m_layer);
  lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), 0);
}

void
Kugelblitz::kill_fall()
{
  explode();
}

void
Kugelblitz::explode()
{
  if (!dying) {
    SoundManager::current()->play("sounds/lightning.wav", m_col.m_bbox.p1());
    set_action("pop");
    lifetime.start(0.2f);
    dying = true;
  }
  else remove_me();
}

void
Kugelblitz::try_activate()
{
  // Define much smaller offscreen distances to appear unexpectedly and surprise Tux.
  float X_OFFSCREEN_DISTANCE = 400;
  float Y_OFFSCREEN_DISTANCE = 600;

  auto player_ = get_nearest_player();
  if (!player_) return;
  Vector dist = player_->get_bbox().get_middle() - m_col.m_bbox.get_middle();
  if ((fabsf(dist.x) <= X_OFFSCREEN_DISTANCE) && (fabsf(dist.y) <= Y_OFFSCREEN_DISTANCE)) {
    set_state(STATE_ACTIVE);
    if (!m_is_initialized) {

      // If the starting direction was set to AUTO, this is our chance to re-orient the badguy.
      if (m_start_dir == Direction::AUTO) {
        Player* player__ = get_nearest_player();
        if (player__ && (player__->get_bbox().get_left() > m_col.m_bbox.get_right())) {
          m_dir = Direction::RIGHT;
        } else {
          m_dir = Direction::LEFT;
        }
      }

      initialize();
      m_is_initialized = true;
    }
    activate();
  }
}

bool
Kugelblitz::is_flammable() const
{
  return false;
}

std::vector<Direction>
Kugelblitz::get_allowed_directions() const
{
  return {};
}

/* EOF */
