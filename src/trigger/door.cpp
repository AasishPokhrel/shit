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

#include "trigger/door.hpp"

#include "audio/sound_manager.hpp"
#include "math/random.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/game_session.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "util/reader_mapping.hpp"

static const float STAY_OPEN_TIME = 1.0f;
static const float LOCK_WARN_TIME = 0.5f;
static const float CENTER_EPSILON = 5.0f;
static const float WALK_SPEED = 100.0f;

Door::Door(const ReaderMapping& mapping) :
  SpritedTrigger(mapping, "images/objects/door/door.sprite", LAYER_BACKGROUNDTILES + 1),
  m_state(CLOSED),
  m_target_sector(),
  m_target_spawnpoint(),
  m_script(),
  m_lock_sprite(SpriteManager::current()->create("images/objects/door/door_lock.sprite")),
  m_stay_open_timer(),
  m_unlocking_timer(),
  m_lock_warn_timer(),
  m_locked(),
  m_lock_color(Color::WHITE),
  m_transition_triggered(false),
  m_triggering_player(nullptr)
{
  mapping.get("sector", m_target_sector);
  mapping.get("spawnpoint", m_target_spawnpoint);
  mapping.get("script", m_script);
  mapping.get("locked", m_locked);

  m_state = m_locked ? DoorState::LOCKED : DoorState::CLOSED;

  set_action("closed");

  std::vector<float> vColor;
  if (mapping.get("lock-color", vColor))
    m_lock_color = Color(vColor);
  else
    m_lock_color = Color::WHITE;
  m_lock_sprite->set_color(m_lock_color);

  SoundManager::current()->preload("sounds/door.wav");
  // TODO: Add proper sounds.
  SoundManager::current()->preload("sounds/locked.ogg");
  SoundManager::current()->preload("sounds/turnkey.ogg");
}

ObjectSettings
Door::get_settings()
{
  ObjectSettings result = SpritedTrigger::get_settings();

  result.add_script(_("Script"), &m_script, "script");
  result.add_text(_("Sector"), &m_target_sector, "sector");
  result.add_text(_("Spawn point"), &m_target_spawnpoint, "spawnpoint");
  result.add_bool(_("Locked?"), &m_locked, "locked");
  result.add_color(_("Lock Color"), &m_lock_color, "lock-color", Color::WHITE);

  result.reorder({"sector", "lock-color", "locked", "spawnpoint", "name", "x", "y"});

  return result;
}

void
Door::after_editor_set()
{
  SpritedTrigger::after_editor_set();

  m_lock_sprite->set_color(m_lock_color);
}

void
Door::update(float )
{
  TriggerBase::update();

  switch (m_state) {
    case CLOSED:
      m_transition_triggered = false;
      break;
    case OPENING:
      // If door has finished opening, start timer and keep door open.
      if (m_sprite->animation_done()) {
        m_state = OPEN;
        set_action("open");
        m_stay_open_timer.start(STAY_OPEN_TIME);
        m_transition_triggered = false;
      }
      break;
    case OPEN:
      // If door was open long enough, start closing it.
      if (m_stay_open_timer.check()) {
        m_state = CLOSING;
        set_action("closing", 1);
      }
      break;
    case CLOSING:
      // If door has finished closing, keep it shut.
      if (m_sprite->animation_done()) {
        m_state = CLOSED;
        set_action("closed");
      }
      break;
    case LOCKED:
      if (m_lock_warn_timer.check()) {
        m_lock_warn_timer.stop();
      }
      break;
    case UNLOCKING:
      if (m_unlocking_timer.check())
      {
        Sector::get().add<SpriteParticle>("images/objects/door/door_lock.sprite",
          "default", get_bbox().get_middle(), ANCHOR_MIDDLE, Vector(0.f, -300.f), Vector(0.f, 1000.f), LAYER_OBJECTS - 2, true, m_lock_color);
        m_unlocking_timer.stop();
        m_state = DoorState::CLOSED;
      }
      break;
  }

  if (m_triggering_player)
  {
    // Check if Tux has moved close enough to the door
    const Vector diff_to_center = get_bbox().get_middle() - m_triggering_player->get_bbox().get_middle();
    if (fabs(diff_to_center.x) < CENTER_EPSILON)
    {
      m_triggering_player->walk(0.0f);
      m_triggering_player = nullptr;
    }
  }
}

void
Door::draw(DrawingContext& context)
{
  MovingSprite::draw(context);
  if (m_state == DoorState::LOCKED || m_state == DoorState::UNLOCKING)
  {
    Vector shake_delta = Vector(static_cast<float>(graphicsRandom.rand(-8, 8)), static_cast<float>(graphicsRandom.rand(-8, 8)));
    float shake_strength = m_lock_warn_timer.started() ? m_lock_warn_timer.get_timeleft() : 0.f;
    m_lock_sprite->draw(context.color(), get_bbox().get_middle() -
      (Vector(m_lock_sprite->get_width() / 2, m_lock_sprite->get_height() / 2) + (shake_delta*shake_strength)), m_layer, m_flip);
  }
}

void
Door::event(Player& player, EventType type)
{
  // If the player is no longer touching the door, they should stop walking.
  if (type == EVENT_LOSETOUCH && m_triggering_player == &player)
  {
    m_triggering_player->walk(0.0f);
    m_triggering_player = nullptr;
    return;
  }

  switch (m_state) {
    case CLOSED:
      // If door was activated, start opening it.
      if (type == EVENT_ACTIVATE) {
        m_state = OPENING;
        SoundManager::current()->play("sounds/door.wav", get_pos());
        set_action("opening", 1);
      }
      break;
    case OPENING:
      break;
    case OPEN:
      break;
    case CLOSING:
      break;
    case LOCKED:
      SoundManager::current()->play("sounds/locked.ogg", get_pos());
      m_lock_warn_timer.start(LOCK_WARN_TIME);
      break;
    case UNLOCKING:
      m_state = CLOSED;
      break;
  }
}

HitResponse
Door::collision(MovingObject& other, const CollisionHit& hit_)
{
  switch (m_state) {
    case CLOSED:
      break;
    case OPENING:
    {
      // If door is opening and was touched by a player, teleport the player.
      Player* player = dynamic_cast<Player*>(&other);

      if (player)
      {
        if (!m_transition_triggered)
        {
          m_transition_triggered = true;
          m_triggering_player = player;

          if (!m_script.empty()) {
            Sector::get().run_script(m_script, "Door");
          }
          if (!m_target_sector.empty())
          {
            // Disable controls, GameSession will make safe Tux during fade animation.
            // Controls will be reactivated after spawn
            m_triggering_player->deactivate();

            // Check if Tux should move a bit closer to the door so that he could go through smoothly
            const Vector diff_to_center = get_bbox().get_middle() - m_triggering_player->get_bbox().get_middle();
            if (fabs(diff_to_center.x) >= CENTER_EPSILON)
            {
              const bool move_right = diff_to_center.x > 0.0f;
              m_triggering_player->set_dir(move_right);
              m_triggering_player->walk(move_right ? WALK_SPEED : -WALK_SPEED);
            }

            GameSession::current()->respawn_with_fade(m_target_sector,
                                                      m_target_spawnpoint,
                                                      ScreenFade::FadeType::CIRCLE,
                                                      get_bbox().get_middle(),
                                                      true);
          }
        }
      }
      break;
    }
    case OPEN:
    case CLOSING:
    case LOCKED:
    case UNLOCKING:
      break;
  }

  return TriggerBase::collision(other, hit_);
}

void
Door::on_flip(float height)
{
  MovingObject::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

void
Door::unlock()
{
  m_locked = false;
  SoundManager::current()->play("sounds/turnkey.ogg", get_pos());
  m_unlocking_timer.start(1.f);
  m_state = DoorState::UNLOCKING;
}

/* EOF */
