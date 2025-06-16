//  SuperTux -- Explosion object
//  Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
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

#include "object/explosion.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "badguy/walking_badguy.hpp"
#include "math/random.hpp"
#include "object/bonus_block.hpp"
#include "object/brick.hpp"
#include "object/camera.hpp"
#include "object/particles.hpp"
#include "object/player.hpp"
#include "object/weak_block.hpp"
#include "supertux/sector.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

Explosion::Explosion(const Vector& pos, float p_push_strength,
    int p_num_particles, bool p_short_fuse) :
  MovingSprite(pos, "images/objects/explosion/explosion.sprite", LAYER_OBJECTS + 40, COLGROUP_MOVING),
  hurt(!p_short_fuse),
  push_strength(p_push_strength),
  num_particles(p_num_particles),
  m_state(E_STATE_WAITING),
  m_lightsprite(SpriteManager::current()->create(p_short_fuse ?
                                                 "images/objects/lightmap_light/lightmap_light-medium.sprite" :
                                                 "images/objects/lightmap_light/lightmap_light-large.sprite")),
  m_color(1.f, 0.5f, 0.2f, 0.f),
  m_fading_timer(),
  short_fuse(p_short_fuse)
{
  set_pos(get_pos() - (m_col.m_bbox.get_middle() - get_pos()));

  SoundManager::current()->preload(short_fuse ? "sounds/firecracker.ogg" : "sounds/explosion.wav");

  m_lightsprite->set_blend(Blend::ADD);
  m_lightsprite->set_color(m_color);
}

Explosion::Explosion(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/explosion/explosion.sprite", LAYER_OBJECTS + 40, COLGROUP_MOVING),
  hurt(true),
  push_strength(-1),
  num_particles(100),
  m_state(E_STATE_WAITING),
  m_lightsprite(nullptr),
  m_color(1.f, 0.5f, 0.2f, 0.f),
  m_fading_timer(),
  short_fuse(false)
{
  SoundManager::current()->preload(short_fuse ? "sounds/firecracker.ogg" : "sounds/explosion.wav");

  m_lightsprite = (SpriteManager::current()->create(short_fuse ?
                                                    "images/objects/lightmap_light/lightmap_light-medium.sprite" :
                                                    "images/objects/lightmap_light/lightmap_light-large.sprite"));
  m_lightsprite->set_blend(Blend::ADD);
  m_lightsprite->set_color(m_color);
}

void
Explosion::explode()
{
  if (m_state != E_STATE_WAITING)
    return;
  m_state = E_STATE_EXPLODING;

  Sector::get().get_camera().shake(.1f, 0.f, 10.f);

  set_action(hurt ? "default" : "pop", 1);
  m_sprite->set_animation_loops(1); //TODO: This is necessary because set_action will not set "loops" when "action" is the default action.
  m_sprite->set_angle(graphicsRandom.randf(0, 360)); // A random rotation on the sprite to make explosions appear more random.
  if (hurt)
    SoundManager::current()->play("sounds/explosion.wav", get_pos(), 0.98f);
  else
    SoundManager::current()->play("sounds/firecracker.ogg", get_pos(), 0.7f);
  bool does_push = push_strength > 0;

  // Spawn some particles.
  Vector accel = Vector(0, Sector::get().get_gravity()*100);
  Sector::get().add<Particles>(
    m_col.m_bbox.get_middle(), -360, 360, 450.0f, 900.0f, accel, num_particles,
    Color(.4f, .4f, .4f), 3, .8f, LAYER_OBJECTS-1);

  if (does_push) {
    Vector center = m_col.m_bbox.get_middle ();
    auto near_objects = Sector::get().get_nearby_objects (center, 128.0 * 32.0);

    for (auto& obj: near_objects) {
      if(!Sector::current()->free_line_of_sight(center, obj->get_pos(), true))
        continue;

      Vector obj_vector = obj->get_bbox ().get_middle ();
      Vector direction = obj_vector - center;
      float distance = glm::length(direction);

      /* If the distance is very small, for example because "obj" is the badguy
       * causing the explosion, skip this object. */
      if (distance <= 1.0f)
        continue;

      /* The force decreases with the distance squared. In the distance of one
       * tile (32 pixels) you will have a speed increase of 150 pixels/s. */
      float force = push_strength / (distance * distance);
      float force_limit = short_fuse ? 400.f : 200.f;
      // If we somehow get a force of over the limit, keep it at the limit because
      // unexpected behaviour could result otherwise.
      if (force > force_limit)
        force = force_limit;

      Vector add_speed = glm::normalize(direction) * force;

      auto player = dynamic_cast<Player*>(obj);
      if (player && !player->is_stone()) {
        player->add_velocity(add_speed);
      }

      auto badguy = dynamic_cast<WalkingBadguy*>(obj);
      if (badguy && badguy->is_active()) {
        badguy->add_velocity(add_speed);
      }

      bool in_break_range = distance <= 60.f;
      bool in_shake_range = distance <= 100.f;

      auto bonusblock = dynamic_cast<BonusBlock*>(obj);
      if (bonusblock && in_shake_range && hurts()) {
        bonusblock->start_bounce(this);
        if (in_break_range)
          bonusblock->try_open(player);
      }

      auto brick = dynamic_cast<Brick*>(obj);
      if (brick && in_shake_range && hurts()) {
        brick->start_bounce(this);
        if (in_break_range)
          brick->try_break(nullptr);
      }

      auto weakblock = dynamic_cast<WeakBlock*>(obj);
      if (weakblock && in_break_range) {
        weakblock->startBurning();
      }
    }
  }
}

void
Explosion::update(float )
{
  switch (m_state)
  {
    case E_STATE_WAITING:
      explode();
      m_fading_timer.start(0.15f);
      break;

    case E_STATE_EXPLODING:
      m_color.alpha = std::min(m_fading_timer.get_progress(), 1.f);

      if (m_fading_timer.check())
      {
        m_fading_timer.start(short_fuse ? .85f : 1.5f);
        m_state = E_STATE_FADING;
      }

      break;

    case E_STATE_FADING:
      m_color.alpha = std::max(1.f - m_fading_timer.get_progress(), 0.f);

      if (m_fading_timer.check())
      {
        remove_me();
      }

      break;
  }
}

void
Explosion::draw(DrawingContext& context)
{
  m_sprite->draw(context.color(), get_pos(), LAYER_OBJECTS+40);
  m_lightsprite->set_color(m_color);
  m_lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), 0);
}

HitResponse
Explosion::collision(MovingObject& other, const CollisionHit& )
{
  if ((m_state != E_STATE_EXPLODING) || !hurt || m_sprite->get_current_frame() > 8)
    return ABORT_MOVE;

  auto player = dynamic_cast<Player*>(&other);
  if (player != nullptr && !player->is_stone()) {
    player->kill(false);
  }

  auto badguy = dynamic_cast<BadGuy*>(&other);
  if (badguy != nullptr) {
    badguy->kill_fall();
  }

  return ABORT_MOVE;
}

/* EOF */
