//  SuperTux
//  Copyright (C) 2024 MatusGuy <matusguy@supertuxproject.org>
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

#include "badguy/tarantula.hpp"

#include "editor/editor.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/surface.hpp"

// These are divided by the distance between the
// floor and the ceiling
static const float DROP_TIME = 420.f;
static const float RETREAT_TIME = 175.f;

// These aren't touched
static const float HANG_TIME = 0.7f;

static const float HANG_HEIGHT = 70.f;

static const float MOVE_SPEED = 75.f;

static const float APPROACH_RANGE = 8.f*32;
static const float DROP_RANGE = 4.f*32;
static const float DROP_DETECT_RANGE = 1200.f;
static const float RETREAT_RANGE = 4.f*32;

Tarantula::Tarantula(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/tarantula/tarantula.sprite"),
  m_state(STATE_IDLE),
  m_timer(),
  m_silk(Surface::from_file("images/creatures/tarantula/silk.png")),
  m_target_height(0),
  m_last_height(0),
  m_was_grabbed(false),
  m_retreat(true),
  m_attach_ceiling(false),
  m_static(false),
  m_ground_height(0.f)
{
  parse_type(reader);
  set_action("idle");

  reader.get("static", m_static, false);

  m_physic.enable_gravity(false);
}

void
Tarantula::initialize()
{
  m_last_height = m_start_position.y;
  Rectf ceiling(Vector(get_bbox().get_left(), get_bbox().get_top() - 2.5f),
                Sizef(get_bbox().get_width(), 2.5f));
  m_attach_ceiling = !Sector::get().is_free_of_tiles(ceiling);
}

bool
Tarantula::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return BadGuy::collision_squished(object);

  set_action("squished");
  kill_squished(object);
  return true;
}

void
Tarantula::active_update(float dt_sec)
{
  BadGuy::active_update(dt_sec);

  if (is_grabbed())
  {
    m_was_grabbed = true;
    return;
  }

  Player* player = get_nearest_player();
  if (player)
  {
    if (!Sector::get().is_free_of_statics(Rectf(Vector(player->get_bbox().get_left(),
                                                       player->get_bbox().get_bottom()+1.f),
                                                Sizef(player->get_bbox().get_width(), 1.f))))
    {
      m_ground_height = player->get_bbox().get_top();
    }

    if (m_state == STATE_HANG_UP || m_state == STATE_HANG_DOWN)
    {
      float dist = get_bbox().get_left() - player->get_bbox().get_left();
      if (std::abs(dist) > RETREAT_RANGE)
      {
        m_retreat = true;
      }
    }
  }

  switch (m_state)
  {
    case STATE_IDLE:
      m_retreat = false;
      [[fallthrough]];
    case STATE_APPROACHING:
      switch (try_approach())
      {
        case NONE:
          m_physic.set_velocity_x(0.f);
          break;

        case DROP:
          m_physic.set_velocity_x(0.f);
          try_drop();
          break;

        default:
          break;
      }

      break;

    case STATE_DROPPING:
      // Doesn't matter what time we put here, because the timer has already started.
      hang_to(m_target_height, 0.f, false, STATE_HANG_UP, EaseQuadIn, "dive");
      break;

    case STATE_HANG_UP:
      hang_to(m_last_height - HANG_HEIGHT, HANG_TIME, false, STATE_HANG_DOWN, EaseSineInOut, "rebound");
      break;

    case STATE_HANG_DOWN:
    {
      bool finished = hang_to(m_last_height + HANG_HEIGHT, HANG_TIME, false, STATE_HANG_UP, EaseSineInOut, "dive");
      if (m_retreat && finished)
      {
        m_state = STATE_RETREATING;
        [[fallthrough]];
      }
      else
      {
        break;
      }
    }

    case STATE_RETREATING:
      hang_to(m_start_position.y, RETREAT_TIME, true, STATE_IDLE, EaseQuadIn, "rebound");
      break;

    default:
      break;
  }
}

Tarantula::ApproachResponse
Tarantula::try_approach()
{
  Player* player = get_nearest_player();
  if (!player)
    return NONE;

  if (get_bbox().get_bottom() >= player->get_bbox().get_top())
    return NONE;

  Vector eye(get_bbox().get_middle().x, get_bbox().get_bottom() + 1);
  float dist = get_bbox().get_left() - player->get_bbox().get_left();

  if (std::abs(dist) > APPROACH_RANGE)
    return NONE;

  if (!Sector::get().can_see_player(eye))
    return NONE;

  if (std::abs(dist) <= DROP_RANGE)
    return DROP;

  if (m_static)
    return NONE;

  if (m_attach_ceiling)
  {
    Vector pos(get_bbox().get_left(), get_bbox().get_top() - 32.f);
    if (dist < 0)
      pos.x += 32.f;

    if (Sector::get().is_free_of_tiles(Rectf(pos, Sizef(2.5f, 32.f))))
      return NONE;
  }

  if (dist > 0)
    m_physic.set_velocity_x(-MOVE_SPEED);
  else if (dist < 0)
    m_physic.set_velocity_x(MOVE_SPEED);

  m_state = STATE_APPROACHING;
  return APPROACH;
}

bool
Tarantula::try_drop()
{
  using RaycastResult = CollisionSystem::RaycastResult;

  // Assuming the player has already been checked...

  Vector eye(get_bbox().get_middle().x, get_bbox().get_bottom() + 1);
  RaycastResult result = Sector::get().get_first_line_intersection(eye,
                                                                   Vector(eye.x, eye.y + DROP_DETECT_RANGE),
                                                                   true,
                                                                   nullptr);

  if (!result.is_valid)
  {
    float sectorheight = static_cast<float>(Sector::get().get_editor_size().height * 32);
    if (sectorheight <= eye.y + DROP_DETECT_RANGE + 1.f)
    {
      // Out of bounds. Drop to the lowest point possible by faking
      // a raycast result.
      if (m_ground_height < 0.f)
        m_ground_height = sectorheight;
      result.box = Rectf(Vector(0.f, m_ground_height), Sizef(1.f, 1.f));
    }
    else
    {
      return false;
    }
  }

  m_state = STATE_DROPPING;
  m_target_height = std::max(result.box.get_top() - 16.f - get_bbox().get_height(), 0.f);
  m_timer.start(calculate_time(DROP_TIME));

  return true;
}

bool
Tarantula::hang_to(float height, float time, bool calctime, State nextstate, EasingMode easing, const std::string& action)
{
  set_action(action, 0);

  bool is_newstate = (m_target_height != height);
  m_target_height = height;

  if (!m_timer.started())
  {
    if (!is_newstate)
    {
      // Timer finished
      m_state = nextstate;
      m_last_height = height;
      return true;
    }
    else
    {
      m_timer.start(calctime ? calculate_time(time) : time);
    }
  }

  double progress = static_cast<double>(m_timer.get_timegone() / m_timer.get_period());
  float offset = static_cast<float>(getEasingByName(easing)(progress)) * (m_target_height - m_last_height);

  Vector pos(get_bbox().get_left(), m_last_height + offset);
  set_pos(pos);

  return false;
}

float
Tarantula::calculate_time(float div)
{
  if (div <= 0.f)
    return 0.f;

  return std::abs(m_target_height - get_bbox().get_top()) / div;
}

void
Tarantula::collision_solid(const CollisionHit& hit)
{
  BadGuy::collision_solid(hit);

  if (hit.top)
    m_attach_ceiling = true;

  if (m_was_grabbed && m_frozen && hit.bottom)
    kill_fall();
}

void
Tarantula::draw(DrawingContext& context)
{
  BadGuy::draw(context);

  if (BadGuy::get_state() == STATE_FALLING ||
      BadGuy::get_state() == STATE_SQUISHED ||
      Editor::is_active())
    return;

  Vector pos(get_bbox().get_left() + ((get_bbox().get_width() - static_cast<float>(m_silk->get_width()))/2),
             m_start_position.y - 32.f);

  float length = std::floor((get_bbox().get_top() - m_start_position.y) / static_cast<float>(m_silk->get_height()));
  for (int i = 0; i <= static_cast<int>(length) + 1; i++) {
    context.color().draw_surface(m_silk, pos, LAYER_TILES-5);
    pos.y += 32.f;
  }
}

void
Tarantula::freeze()
{
  m_physic.enable_gravity(true);
  BadGuy::freeze();
}

void
Tarantula::unfreeze(bool)
{
  kill_fall();
}

bool
Tarantula::is_freezable() const
{
  return true;
}

bool
Tarantula::is_snipable() const
{
  return m_state != STATE_DROPPING;
}

GameObjectTypes
Tarantula::get_types() const
{
  return {
    { "tarantula", _("Tarantula") },
    { "spidermite", _("Spidermite") }
  };
}

std::string
Tarantula::get_default_sprite_name() const
{
  switch (m_type)
  {
    case TARANTULA: return "images/creatures/tarantula/tarantula.sprite";
    case SPIDERMITE: return "images/creatures/spidermite/spidermite.sprite";
  }
  return "images/creatures/tarantula/tarantula.sprite";
}

ObjectSettings
Tarantula::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_bool(_("Static"), &m_static, "static", false);

  return result;
}

std::vector<Direction>
Tarantula::get_allowed_directions() const
{
  return {};
}

/* EOF */
