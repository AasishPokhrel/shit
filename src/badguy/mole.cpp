//  SuperTux - Mole Badguy
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "badguy/mole.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "badguy/mole_rock.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"

static const float MOLE_WAIT_TIME = 0.2f; /**< Time to wait before and after throwing. */
static const float THROW_TIME = 4.6f;    /**< Time to spend throwing. */
static const float THROW_INTERVAL = 1;   /**< Time between two thrown rocks. */
static const float THROW_VELOCITY = 400; /**< Initial velocity of thrown rocks. */

Mole::Mole(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/mole/mole.sprite", LAYER_TILES-1),
  state(PRE_THROWING),
  timer(),
  throw_timer(),
  cycle_num()
{
  m_physic.enable_gravity(false);
  SoundManager::current()->preload("sounds/fall.wav");
  SoundManager::current()->preload("sounds/squish.wav");
  SoundManager::current()->preload("sounds/dartfire.wav");
}

void
Mole::activate()
{
  if (state != DEAD) set_state(PRE_THROWING);
}

void
Mole::kill_fall()
{
  if (state == DEAD)
    return;

  set_state(DEAD);
  SoundManager::current()->play("sounds/fall.wav", get_pos());
  run_dead_script();
}

HitResponse
Mole::collision_badguy(BadGuy& , const CollisionHit& )
{
  return FORCE_MOVE;
}

bool
Mole::collision_squished(MovingObject& obj)
{
  set_state(DEAD);

  if (auto player = dynamic_cast<Player*>(&obj))
    player->bounce(*this);

  SoundManager::current()->play("sounds/squish.wav", get_pos());
  run_dead_script();
  return true;
}

void
Mole::throw_rock()
{
  float angle = math::radians(135.f - (static_cast<float>(cycle_num) * 30.f));

  SoundManager::current()->play("sounds/dartfire.wav", get_pos());
  Sector::get().add<MoleRock>(m_col.m_bbox.get_middle(),
    THROW_VELOCITY * ((cycle_num == 0 || cycle_num == 3) ? 0.8f : 1.f) *
    Vector(cosf(angle), sin(angle) * (m_flip == NO_FLIP ? -1.f : 1.f)), this);
  cycle_num += 1;
}

void
Mole::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);

  switch (state) {
    case PRE_THROWING:
      if (timer.check()) {
        set_state(THROWING);
      }
      break;
    case THROWING:
      if (throw_timer.check()) {
        throw_rock();
        throw_timer.start(THROW_INTERVAL);
      }
      if (timer.check()) {
        set_state(POST_THROWING);
      }
      break;
    case POST_THROWING:
      if (timer.check()) {
        set_state(PEEKING);
      }
      break;
    case PEEKING:
      if (m_sprite->animation_done()) {
        set_state(PRE_THROWING);
      }
      break;
    case BURNING:
      if (m_sprite->animation_done()) {
        set_state(DEAD);
      }
      break;
    case DEAD:
      break;
  }

}

void
Mole::set_state(MoleState new_state)
{
  switch (new_state) {
    case PRE_THROWING:
      set_action("idle");
      set_colgroup_active(COLGROUP_DISABLED);
      timer.start(MOLE_WAIT_TIME);
      break;
    case THROWING:
      set_action("idle");
      set_colgroup_active(COLGROUP_DISABLED);
      timer.start(THROW_TIME);
      throw_timer.start(THROW_INTERVAL);
      break;
    case POST_THROWING:
      cycle_num = 0;
      set_action("idle");
      set_colgroup_active(COLGROUP_DISABLED);
      timer.start(MOLE_WAIT_TIME);
      break;
    case PEEKING:
      set_action("peeking", 1);
      set_colgroup_active(COLGROUP_STATIC);
      break;
    case DEAD:
      set_action("squished");
      set_colgroup_active(COLGROUP_DISABLED);
      break;
    case BURNING:
      set_action("burning", 1);
      set_colgroup_active(COLGROUP_DISABLED);
      break;
  }

  state = new_state;
}

void
Mole::ignite() {
  set_state(BURNING);
  run_dead_script();
  SoundManager::current()->play("sounds/fire.ogg", get_pos());
}

std::vector<Direction>
Mole::get_allowed_directions() const
{
  return {};
}

void
Mole::on_flip(float height)
{
  BadGuy::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

/* EOF */
