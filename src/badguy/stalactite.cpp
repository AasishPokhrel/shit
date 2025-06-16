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

#include "badguy/stalactite.hpp"

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "math/random.hpp"
#include "object/bullet.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

static const int SHAKE_RANGE_X = 40;
static const float SHAKE_TIME = .8f;
static const float SHAKE_RANGE_Y = 400;

Stalactite::Stalactite(const ReaderMapping& mapping) :
  StickyBadguy(mapping, "images/creatures/stalactite/stalactite_ice.sprite", LAYER_TILES - 1, COLGROUP_MOVING),
  timer(),
  state(STALACTITE_HANGING),
  shake_delta(0.0f, 0.0f)
{
  parse_type(mapping);

  if (m_type != StalactiteType::ICE)
    after_editor_set();

  m_countMe = false;
  set_colgroup_active(COLGROUP_TOUCHABLE);
  SoundManager::current()->preload("sounds/cracking.wav");
  SoundManager::current()->preload("sounds/sizzle.ogg");
  SoundManager::current()->preload("sounds/icecrash.ogg");

  mapping.get("sticky", m_sticky, false);
}

void
Stalactite::active_update(float dt_sec)
{
  if (state == STALACTITE_HANGING) {
    auto player = get_nearest_player();
    if (player && !player->get_ghost_mode()) {
      if (player->get_bbox().get_right() > m_col.m_bbox.get_left() - SHAKE_RANGE_X
         && player->get_bbox().get_left() < m_col.m_bbox.get_right() + SHAKE_RANGE_X
         && player->get_bbox().get_bottom() > m_col.m_bbox.get_top()
         && player->get_bbox().get_top() < m_col.m_bbox.get_bottom() + SHAKE_RANGE_Y
         && Sector::get().can_see_player(m_col.m_bbox.get_middle())) {
        timer.start(SHAKE_TIME);
        state = STALACTITE_SHAKING;
        SoundManager::current()->play("sounds/cracking.wav", get_pos());
      }
    }
  } else if (state == STALACTITE_SHAKING) {
    shake_delta = Vector(static_cast<float>(graphicsRandom.rand(-3, 3)), 0.0f);
    if (timer.check()) {
      state = STALACTITE_FALLING;
      m_physic.enable_gravity(true);
      set_colgroup_active(COLGROUP_MOVING);
    }
  } else if (state == STALACTITE_FALLING) {
    m_col.set_movement(m_physic.get_movement(dt_sec));
  }

  if (state != STALACTITE_FALLING && m_sticky) {
    sticky_update(dt_sec);
  }
}

void
Stalactite::squish()
{
  state = STALACTITE_SQUISHED;
  m_physic.enable_gravity(true);
  m_physic.set_velocity(0, 0);
  set_state(STATE_SQUISHED);
  set_action("squished");
  SoundManager::current()->play("sounds/icecrash.ogg", get_pos());
  set_group(COLGROUP_MOVING_ONLY_STATIC);
  run_dead_script();
}

void
Stalactite::collision_solid(const CollisionHit& hit)
{
  if (state == STALACTITE_FALLING) {
    if (hit.bottom) squish();
  }
}

HitResponse
Stalactite::collision_player(Player& player, const CollisionHit& )
{
  if (state != STALACTITE_SQUISHED) {
    player.kill(false);
  }

  return FORCE_MOVE;
}

HitResponse
Stalactite::collision_badguy(BadGuy& other, const CollisionHit& hit)
{
  if (state == STALACTITE_SQUISHED) return ABORT_MOVE;

  // Ignore other Stalactites.
  if (dynamic_cast<Stalactite*>(&other)) return FORCE_MOVE;

  if (state != STALACTITE_FALLING) return BadGuy::collision_badguy(other, hit);

  if (other.is_freezable() && m_type != StalactiteType::ROCK) {
    other.freeze();
  } else {
    other.kill_fall();
  }

  return FORCE_MOVE;
}

HitResponse
Stalactite::collision_bullet(Bullet& bullet, const CollisionHit& hit)
{
  if (m_type == StalactiteType::ROCK)
  {
    bullet.ricochet(*this, hit);
  }
  else if (state == STALACTITE_HANGING)
  {
    timer.start(SHAKE_TIME);
    state = STALACTITE_SHAKING;
    bullet.remove_me();
    if (bullet.get_type() == BONUS_FIRE)
      SoundManager::current()->play("sounds/sizzle.ogg", get_pos());
    SoundManager::current()->play("sounds/cracking.wav", get_pos());
  }

  return FORCE_MOVE;
}

GameObjectTypes
Stalactite::get_types() const
{
  return {
    { "ice", _("ice") },
    { "rock", _("rock") }
  };
}

std::string
Stalactite::get_default_sprite_name() const
{
  switch (m_type)
  {
    case ROCK:
      return "images/creatures/stalactite/stalactite_rock.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
Stalactite::kill_fall()
{
}

void
Stalactite::draw(DrawingContext& context)
{
  if (Editor::is_active()) {
    BadGuy::draw(context);
    return;
  }

  if (get_state() == STATE_INIT || get_state() == STATE_INACTIVE)
    return;

  if (state == STALACTITE_SQUISHED) {
    m_sprite->draw(context.color(), get_pos(), LAYER_OBJECTS);
  } else if (state == STALACTITE_SHAKING) {
    m_sprite->draw(context.color(), get_pos() + shake_delta, m_layer, m_flip);
  } else {
    m_sprite->draw(context.color(), get_pos(), m_layer, m_flip);
  }
}

void
Stalactite::deactivate()
{
  if (state != STALACTITE_HANGING)
    remove_me();
}

std::vector<Direction>
Stalactite::get_allowed_directions() const
{
  return {};
}

void
Stalactite::on_flip(float height)
{
  BadGuy::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

ObjectSettings
Stalactite::get_settings()
{
  ObjectSettings result = StickyBadguy::get_settings();

  result.reorder({"sticky", "speed", "sprite", "x", "y" });

  return result;
}

/* EOF */
