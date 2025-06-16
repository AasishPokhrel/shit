//  SuperTux
//  Copyright (C) 2023 MatusGuy
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

#include "badguy/corrupted_granito_big.hpp"

#include "audio/sound_manager.hpp"
#include "collision/collision_system.hpp"
#include "math/random.hpp"
#include "object/player.hpp"
#include "object/shard.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

static const std::string SHARD_SPRITE = "images/creatures/granito/corrupted/big/root_spike.sprite";
static const float RANGE = 5; // tiles
static const float CRACK_TIME = 1.f; // seconds
static const float SHAKE_TIME = 0.1f; // seconds

CorruptedGranitoBig::CorruptedGranitoBig(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/granito/corrupted/big/rock_mine.sprite", LAYER_OBJECTS-3),
  m_state(STATE_READY),
  m_crack_timer(),
  m_shake_timer(),
  m_shake_delta(0.f),
  m_rock_particles(SpriteManager::current()->create("images/particles/granito_piece.sprite"))
{
  parse_type(reader);

  m_col.set_unisolid(true);
  m_physic.enable_gravity(false);

  SoundManager::current()->preload("sounds/brick.wav");
}

void
CorruptedGranitoBig::initialize()
{
  BadGuy::initialize();

  m_state = STATE_READY;
  set_action("idle", m_dir);
  set_colgroup_active(COLGROUP_STATIC);
}

void
CorruptedGranitoBig::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);

  if (m_state == STATE_BROKEN) return;

  const bool crack = try_cracking();
  if (!crack && !m_crack_timer.paused() && m_crack_timer.started()) m_crack_timer.pause();

  if (m_shake_timer.started())
    m_shake_delta = static_cast<float>(graphicsRandom.rand(-3, 3));
  else
    m_shake_delta = 0.f;
}

void
CorruptedGranitoBig::draw(DrawingContext &context)
{
  m_sprite->draw(context.color(), {get_pos().x + m_shake_delta, get_pos().y}, m_layer, m_flip);
}

void
CorruptedGranitoBig::kill_fall()
{
  if (m_state == STATE_BROKEN)
    return;

  m_state = STATE_BROKEN;
  set_action("broken", m_dir);
  set_colgroup_active(COLGROUP_DISABLED);
  m_col.set_unisolid(false);

  run_dead_script();

  Sector::get().add<Shard>(get_bbox().get_middle(), Vector(100.f, -500.f), SHARD_SPRITE);
  Sector::get().add<Shard>(get_bbox().get_middle(), Vector(270.f, -350.f), SHARD_SPRITE);
  Sector::get().add<Shard>(get_bbox().get_middle(), Vector(-100.f, -500.f),SHARD_SPRITE);
  Sector::get().add<Shard>(get_bbox().get_middle(), Vector(-270.f, -350.f),SHARD_SPRITE);

  crack_effects(6);
}

bool
CorruptedGranitoBig::try_cracking()
{
  using RaycastResult = CollisionSystem::RaycastResult;

  Player* player = get_nearest_player();
  if (!player) return false;

  const Vector middle = get_bbox().get_middle();
  const Vector player_middle = player->get_bbox().get_middle();

  RaycastResult result = Sector::get().get_first_line_intersection(middle, player_middle,
                                                                   false, get_collision_object());

  auto* obj = std::get_if<CollisionObject*>(&result.hit);
  if (!obj || *obj != player->get_collision_object()) return false;

  if (glm::distance(middle, player_middle) > 32.f * RANGE) return false;

  crack();
  return true;
}

void
CorruptedGranitoBig::crack()
{
  if (m_state == STATE_CRACK2 && m_crack_timer.check())
  {
    kill_fall();
    return;
  }

  if (m_crack_timer.paused())
    m_crack_timer.resume();
  else if (!m_crack_timer.started())
    m_crack_timer.start(CRACK_TIME);

  float progress = m_crack_timer.get_progress();

  if (m_state == STATE_CRACK1 && progress >= 0.5f)
  {
    m_state = STATE_CRACK2;
    set_action("cracked-2", m_dir);
    crack_effects(4);
  }
  else if (m_state == STATE_READY)
  {
    m_state = STATE_CRACK1;
    set_action("cracked-1", m_dir);
    crack_effects(3);
  }
}

void
CorruptedGranitoBig::crack_effects(int particles)
{
  assert(particles > 0 && particles <= 6);

  SoundManager::current()->play("sounds/brick.wav", get_pos());
  m_shake_timer.start(SHAKE_TIME);

  const float gravity = Sector::get().get_gravity() * 100.f;
  for (int i = 0; i < particles; i++)
  {
    const Vector velocity(graphicsRandom.randf(-100, 100),
                          graphicsRandom.randf(-400, -300));
    Sector::get().add<SpriteParticle>(m_rock_particles->clone(), "piece-" + std::to_string(i),
                                      get_bbox().get_middle(), ANCHOR_MIDDLE,
                                      velocity, Vector(0, gravity),
                                      LAYER_OBJECTS + 3, true);
  }
}

/* EOF */
