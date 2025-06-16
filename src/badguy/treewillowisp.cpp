//  SuperTux - "Will-O-Wisp" Badguy
//  Copyright (C) 2007 Matthias Braun
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

#include "badguy/treewillowisp.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "badguy/dispenser.hpp"
#include "badguy/ghosttree.hpp"
#include "math/util.hpp"
#include "object/lantern.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"

static const std::string TREEWILLOSOUND = "sounds/willowisp.wav";

TreeWillOWisp::TreeWillOWisp(GhostTree* tree_, const Vector& pos,
                             float radius_, float speed_) :
  BadGuy(tree_->get_pos() + pos, "images/creatures/willowisp/willowisp.sprite",
         LAYER_OBJECTS - 20),
  was_sucked(false),
  mystate(STATE_DEFAULT),
  color(),
  angle(0),
  radius(radius_),
  speed(speed_),
  sound_source(),
  tree(tree_),
  suck_target(0.0f, 0.0f)
{
  SoundManager::current()->preload(TREEWILLOSOUND);
  set_colgroup_active(COLGROUP_MOVING);
}

TreeWillOWisp::~TreeWillOWisp()
{
}

void
TreeWillOWisp::activate()
{
  sound_source = SoundManager::current()->create_sound_source(TREEWILLOSOUND);
  sound_source->set_position(get_pos());
  sound_source->set_looping(true);
  sound_source->set_gain(1.0f);
  sound_source->set_reference_distance(32);
  sound_source->play();
}

void
TreeWillOWisp::vanish()
{
  mystate = STATE_VANISHING;
  set_action("vanishing", 1);
  set_colgroup_active(COLGROUP_DISABLED);

  if (m_parent_dispenser != nullptr)
  {
    m_parent_dispenser->notify_dead();
  }
}

void
TreeWillOWisp::start_sucking(const Vector& suck_target_)
{
  mystate = STATE_SUCKED;
  suck_target = suck_target_;
  was_sucked = true;
}

HitResponse
TreeWillOWisp::collision_player(Player& player, const CollisionHit& hit)
{
  // TODO: This function is essentially a no-op. Remove if it doesn't change the behavior.
  return BadGuy::collision_player(player, hit);
}

bool
TreeWillOWisp::collides(MovingObject& other, const CollisionHit& ) const
{
  auto lantern = dynamic_cast<Lantern*>(&other);
  if (lantern && lantern->is_open())
    return true;
  if (dynamic_cast<Player*>(&other))
    return true;

  return false;
}

void
TreeWillOWisp::draw(DrawingContext& context)
{
  m_sprite->draw(context.color(), get_pos(), m_layer);
  m_sprite->draw(context.light(), get_pos(), m_layer);
}

void
TreeWillOWisp::active_update(float dt_sec)
{
  // Remove the TreeWillOWisp if it has completely vanished.
  if (mystate == STATE_VANISHING) {
    if (m_sprite->animation_done()) {
      remove_me();
      tree->willowisp_died(this);
    }
    return;
  }

  if (mystate == STATE_SUCKED) {
    Vector dir_ = suck_target - get_pos();
    if (glm::length(dir_) < 5) {
      vanish();
      return;
    }
    Vector newpos = get_pos() + dir_ * dt_sec;
    m_col.set_movement(newpos - get_pos());
    return;
  }

  angle = fmodf(angle + dt_sec * speed, math::TAU);
  Vector newpos(m_start_position + Vector(sinf(angle) * radius, 0));
  m_col.set_movement(newpos - get_pos());
  float sizemod = cosf(angle) * 0.8f;
  /* TODO: Modify sprite size using the 'sizeMod' value. */

  sound_source->set_position(get_pos());

  if (sizemod < 0) {
    m_layer = LAYER_OBJECTS + 5;
  } else {
    m_layer = LAYER_OBJECTS - 20;
  }
}

void
TreeWillOWisp::set_color(const Color& color_)
{
  color = color_;
  m_sprite->set_color(color_);
}

void TreeWillOWisp::stop_looping_sounds()
{
  if (sound_source) {
    sound_source->stop();
  }
}

void TreeWillOWisp::play_looping_sounds()
{
  if (sound_source) {
    sound_source->play();
  }
}

/* EOF */
