//  SuperTux
//  Copyright (C) 2022 Daniel Ward <weluvgoatz@gmail.com>
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

#include "object/key.hpp"

#include "audio/sound_manager.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"
#include "trigger/door.hpp"
#include "util/reader_mapping.hpp"

Key::Key(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/keys/key.sprite", LAYER_OBJECTS, COLGROUP_TOUCHABLE),
  m_pos_list(),
  m_collected(),
  m_state(KeyState::NORMAL),
  m_wait_timer(),
  m_unlock_timer(),
  m_physic(),
  m_chain_pos(1),
  m_my_door_pos(0.f, 0.f),
  m_color(Color::WHITE),
  m_owner(),
  m_lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light-small.sprite")),
  m_total_time_elapsed(),
  m_target_speed()
{
  std::vector<float> vColor;
  if (reader.get("color", vColor)) {
    m_color = Color(vColor);
  }
  m_lightsprite->set_blend(Blend::ADD);
  m_lightsprite->set_color(m_color);

  // TODO: Add proper sound
  SoundManager::current()->preload("sounds/metal_hit.ogg");
  m_sprite->set_color(m_color);
  m_physic.enable_gravity(false);
}

void
Key::update(float dt_sec)
{
  m_target_speed = 12.f * std::cos(m_total_time_elapsed * 2.f);
  if (m_state == KeyState::NORMAL)
  {
    m_total_time_elapsed += dt_sec;
    m_col.set_movement(Vector(0.f, m_target_speed*dt_sec));
  }

  if (!m_owner || !m_owner->is_dead())
  {
    float px = graphicsRandom.randf(get_bbox().get_left(), get_bbox().get_right());
    float py = graphicsRandom.randf(get_bbox().get_top(), get_bbox().get_bottom());
    Vector ppos = Vector(px, py);
    int upper_limit = m_state == KeyState::NORMAL ? 20 : 50;
    bool spawn_particle_now = (graphicsRandom.rand(0, upper_limit) == 5);
    if (spawn_particle_now)
    {
      Sector::get().add<SpriteParticle>(
        "images/particles/sparkle.sprite", "small",
        ppos, ANCHOR_MIDDLE, Vector(0, 0), Vector(0, 0), LAYER_OBJECTS + 6, false, m_color);
    }
  }

  if (!m_owner)
    return;

  float distance = glm::length(get_pos() - m_owner->get_pos());

  if (m_state != KeyState::NORMAL)
  {
    m_pos_list.push_front((m_owner->get_bbox().get_middle())-
      Vector(get_bbox().get_width()/2.f, get_bbox().get_height()/2.f)-
      Vector(0.f, 10.f));
  }
  Vector m_goal_pos = m_pos_list.back();

  while (m_pos_list.size() > unsigned(20 * m_chain_pos))
    m_pos_list.pop_back();

  // use
  for (auto& door : Sector::get().get_objects_by_type<Door>()) {
    if (m_state == KeyState::FOLLOW && door.is_locked() && glm::length(get_pos() - door.get_pos()) < 100.f &&
      // color matches
      std::abs(door.get_lock_color().red - m_color.red) <= 0.1f &&
      std::abs(door.get_lock_color().green - m_color.green) <= 0.1f &&
      std::abs(door.get_lock_color().blue - m_color.blue) <= 0.1f) {
      m_owner->remove_collected_key(this);
      for (auto& key : Sector::get().get_objects_by_type<Key>()) {
        if (key.m_chain_pos > m_chain_pos)
          key.m_chain_pos -= 1;
      }
      door.unlock();
      m_my_door_pos = door.get_bbox().get_middle();
      m_unlock_timer.start(1.f);
      m_state = KeyState::FOUND;
    }
  }

  switch (m_state) {
  case NORMAL:
    break;
  case COLLECT:
    m_col.set_movement(Vector(0, 0));
    if (m_wait_timer.check())
    {
      m_wait_timer.stop();
      m_state = KeyState::FOLLOW;
    }
    break;
  case FOLLOW:
    m_total_time_elapsed += dt_sec;
    m_col.set_movement(((m_goal_pos - get_pos()) * (std::max(0.f, distance-(50.f*float(m_chain_pos)))/300.f))+
      Vector(0.f, m_target_speed * dt_sec));
    if (Sector::get().get_object_count<Player>([](const Player& player) { return !player.is_dead(); }) <= 0)
      m_state = KeyState::FOUND;
    break;
  case FOUND:
    m_col.set_movement((m_my_door_pos - Vector(get_bbox().get_width() / 2.f, get_bbox().get_height() / 2.f) -
      (get_pos()))
      *glm::length((m_my_door_pos - get_bbox().get_middle())*0.003f));
    if (m_unlock_timer.check() || m_owner->is_dying())
    {
      m_unlock_timer.stop();
      m_physic.enable_gravity(true);
      m_physic.set_velocity_y(-300.f);
      m_physic.set_acceleration_y(500.f);
      spawn_use_particles();
      m_state = KeyState::USE;
    }
    break;
  case USE:
    m_col.set_movement(m_physic.get_movement(dt_sec));
    if (get_pos().y > Sector::get().get_height())
      remove_me();
    break;
  }

  GameObject::update(dt_sec);
}

HitResponse
Key::collision(MovingObject& other, const CollisionHit& hit_)
{
  if (m_state == KeyState::NORMAL)
  {
    auto player = dynamic_cast<Player*>(&other);
    if (player)
    {
      spawn_use_particles();
      m_total_time_elapsed = 0.f;
      SoundManager::current()->play("sounds/metal_hit.ogg", get_pos());
      m_chain_pos = static_cast<int>(player->get_collected_keys().size()) + 1;
      player->add_collected_key(this);
      m_collected = true;
      m_wait_timer.start(0.5f);
      m_owner = player;
      m_state = KeyState::COLLECT;
    }
  }
  return FORCE_MOVE;
}

void
Key::draw(DrawingContext& context)
{
  if (m_owner && m_owner->is_dead())
    return;

  m_sprite->draw(context.color(), get_pos(), m_layer, m_flip);
  m_lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), m_layer+1);
}

ObjectSettings
Key::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_color(_("Color"), &m_color, "color", Color::WHITE);

  result.reorder({ "color", "name", "x", "y" });

  return result;
}

void
Key::after_editor_set()
{
  MovingSprite::after_editor_set();

  m_lightsprite->set_blend(Blend::ADD);
  m_lightsprite->set_color(m_color);
  m_sprite->set_color(m_color);
}

void
Key::update_pos()
{
  m_col.m_bbox.set_pos(m_owner->get_bbox().get_middle() -
    Vector(m_col.m_bbox.get_width() / 2.f, m_col.m_bbox.get_height() / 2.f - 10.f));
}

void
Key::spawn_use_particles()
{
  for (int i = 1; i < 9; i++)
  {
    Vector direction = glm::normalize(Vector(std::cos(float(i) * math::PI_4), std::sin(float(i) * math::PI_4)));
    Sector::get().add<SpriteParticle>("images/particles/sparkle.sprite", "small-key-collect",
      get_bbox().get_middle(),
      ANCHOR_MIDDLE, Vector(400.f * direction), -Vector(400.f * direction) * 2.8f, LAYER_OBJECTS + 6, false, m_color);
  }
}

/* EOF */
