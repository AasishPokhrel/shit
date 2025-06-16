//  SuperTux - Unstable Tile
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
//                2023 Vankata453
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

#include "object/unstable_tile.hpp"

#include "math/random.hpp"
#include "object/explosion.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/constants.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"

static const float CRACK_TIME = 0.3f;
static const float FALL_TIME = 0.8f;
static const float RESPAWN_TIME = 5.f;
static const float FADE_OUT_TIME = 1.f;
static const float FADE_IN_TIME = .5f;
static const float DELAY_IF_TUX = 0.001f;

UnstableTile::UnstableTile(const ReaderMapping& mapping, int type) :
  MovingSprite(mapping, "images/objects/unstable_tile/snow.sprite", LAYER_TILES, COLGROUP_STATIC),
  physic(),
  state(STATE_NORMAL),
  slowfall_timer(),
  m_revive_timer(),
  m_respawn(),
  m_alpha(1.f),
  m_original_pos(m_col.get_pos()),
  m_fall_timer(),
  m_player_hit(false)
{
  if (type >= 0)
  {
    m_type = type;
    on_type_change(TypeChange::INITIAL);
  }
  else
  {
    parse_type(mapping);
  }

  set_action("default");

  physic.set_gravity_modifier(.98f);
  physic.enable_gravity(false);
}

GameObjectTypes
UnstableTile::get_types() const
{
  return {
    { "ice", _("Ice") },
    { "brick", _("Brick") },
    { "delayed", _("Delayed") }
  };
}

std::string
UnstableTile::get_default_sprite_name() const
{
  switch (m_type)
  {
    case BRICK:
      return "images/objects/unstable_tile/brick.sprite";
    case DELAYED:
      return "images/objects/skull_tile/skull_tile.sprite";
    default:
      return m_default_sprite_name;
  }
}

HitResponse
UnstableTile::collision(MovingObject& other, const CollisionHit& )
{
  if (state == STATE_NORMAL)
  {
    Player* player = dynamic_cast<Player*>(&other);
    if (player != nullptr &&
       (player->get_bbox().get_bottom() < m_col.m_bbox.get_top() + SHIFT_DELTA ||
       player->get_bbox().get_top() < m_col.m_bbox.get_bottom() + SHIFT_DELTA))
    {
      if (m_type == DELAYED)
        m_player_hit = true;
      else
        shake();
    }

    if (m_type != DELAYED && dynamic_cast<Explosion*>(&other))
    {
      shake();
    }
  }
  return FORCE_MOVE;
}

void
UnstableTile::shake()
{
  if (state != STATE_NORMAL)
    return;

  if (m_sprite->has_action("shake"))
  {
    state = STATE_SHAKE;
    set_action("shake", /* loops = */ 1);
  }
  else
  {
    dissolve();
  }
}

void
UnstableTile::dissolve()
{
  if ((state != STATE_NORMAL) && (state != STATE_SHAKE))
    return;

  if (m_sprite->has_action("dissolve"))
  {
    state = STATE_DISSOLVE;
    set_action("dissolve", /* loops = */ 1);
  }
  else
  {
    slow_fall();
  }
}

void
UnstableTile::slow_fall()
{
  /* Only enter slow-fall if neither shake nor dissolve is available. */
  if (state != STATE_NORMAL)
  {
    fall_down();
    return;
  }

  if (m_sprite->has_action("fall-down"))
  {
    state = STATE_SLOWFALL;
    set_action("fall-down", /* loops = */ 1);
    physic.set_gravity_modifier(.10f);
    physic.enable_gravity(true);
    m_original_pos = m_col.get_pos();
    slowfall_timer = 0.5f; /* Fall slowly for half a second. */
  }
  else
  {
    state = STATE_FALL;
  }
}

void
UnstableTile::fall_down()
{
  if (state == STATE_FALL)
    return;

  state = STATE_FALL;

  const bool has_action = m_sprite->has_action("fall-down");
  if (m_type == DELAYED || has_action)
  {
    if (has_action)
      set_action("fall-down", /* loops = */ 1);
    physic.set_gravity_modifier(.98f);
    physic.enable_gravity(true);
  }
}

void
UnstableTile::revive()
{
  state = STATE_NORMAL;
  set_group(COLGROUP_STATIC);
  physic.enable_gravity(false);
  physic.set_velocity(Vector(0.0f, 0.0f));
  m_col.set_pos(m_original_pos);
  m_col.set_movement(Vector(0.0f, 0.0f));
  m_revive_timer.stop();
  m_respawn.reset(new FadeHelper(&m_alpha, FADE_IN_TIME, 1.f));
  set_action("default");
}

void
UnstableTile::update(float dt_sec)
{
  if (m_respawn)
  {
    m_respawn->update(dt_sec);

    if (m_respawn->completed())
      m_respawn.reset();
  }

  switch (state)
  {
    case STATE_NORMAL:
      if (m_type != DELAYED)
        break;

      /** Manage DELAYED type (behaviour of the former SkullTile object). */

      if (m_player_hit)
      {
        set_action("mad");
        if (m_fall_timer.check())
          fall_down();
        else if (!m_fall_timer.started())
          m_fall_timer.start(FALL_TIME);
        else if (m_fall_timer.get_timegone() > CRACK_TIME) // Should perform shake animation.
          m_col.set_pos(m_original_pos + Vector(static_cast<float>(graphicsRandom.rand(-3, 3)), 0.f));
      }
      else
      {
        set_action("default");
        m_fall_timer.stop();
        m_col.set_pos(m_original_pos);
      }

      m_player_hit = false; // To be updated next frame in collision().
      break;

    case STATE_SHAKE:
      if (m_sprite->animation_done())
        dissolve();
      break;

    case STATE_DISSOLVE:
      if (m_sprite->animation_done()) {
        /* dissolving is done. Set to non-solid. */
        set_group(COLGROUP_DISABLED);
        fall_down();
      }
      break;

    case STATE_SLOWFALL:
      if (slowfall_timer >= dt_sec)
	      slowfall_timer -= dt_sec;
      else /* Switch to normal falling procedure */
	      fall_down();
      m_col.set_movement(physic.get_movement(dt_sec));
      break;

    case STATE_FALL:
      // TODO: A state enum for when the tile is "dead"?
      m_alpha = std::max(m_alpha - dt_sec / FADE_OUT_TIME, 0.f);
      if (!m_revive_timer.started())
      {
        if (m_revive_timer.check())
        {
          if (Sector::get().is_free_of_movingstatics(Rectf(m_original_pos, get_bbox().get_size()).grown(-1.f)))
          {
            revive();
          }
          else
          {
            m_revive_timer.start(DELAY_IF_TUX);
          }
        }
        else
        {
          m_revive_timer.start(RESPAWN_TIME);
        }
      }
      else if (m_alpha > 0.f)
      {
        m_col.set_movement(physic.get_movement(dt_sec));
      }
      else
      {
        set_group(COLGROUP_DISABLED);
      }
      break;
  }
}

void
UnstableTile::draw(DrawingContext& context)
{
  // FIXME: This method is more future-proof, but more ugly than simply copying
  //        the draw() function from MovingSprite
  context.push_transform();
  context.transform().alpha *= m_alpha;
  MovingSprite::draw(context);
  context.pop_transform();
}

void
UnstableTile::on_flip(float height)
{
  MovingObject::on_flip(height);
  m_original_pos.y = height - m_original_pos.y - get_bbox().get_height();
  FlipLevelTransformer::transform_flip(m_flip);
}

/* EOF */
