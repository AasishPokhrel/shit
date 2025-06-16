//  SuperTux
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

#include "badguy/ghoul.hpp"

#include "object/player.hpp"
#include "editor/editor.hpp"
#include "sprite/sprite.hpp"
#include "supertux/game_session.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

static const float FLYSPEED = 80.0f; /**< Speed in px per second. */
static const float TRACK_RANGE = 2500.0f; /**< At what distance to start tracking the player. */

Ghoul::Ghoul(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/ghoul/ghoul.sprite"),
  PathObject(),
  m_mystate(STATE_IDLE),
  m_flyspeed(),
  m_track_range()
{
  reader.get("flyspeed", m_flyspeed, FLYSPEED);
  reader.get("track-range", m_track_range, TRACK_RANGE);
  
  bool running;
  reader.get("running", running, false);

  init_path(reader, running);

  set_action(m_dir);
}

bool
Ghoul::collision_squished(MovingObject& object)
{
  auto player = Sector::get().get_nearest_player(m_col.m_bbox);
  if (player)
    player->bounce (*this);
  set_action("squished", 1);
  kill_fall();
  return true;
}

bool
Ghoul::is_freezable() const
{
  return false;
}

bool
Ghoul::is_flammable() const
{
  return false;
}

void
Ghoul::finish_construction()
{
  if (get_walker() && get_walker()->is_running()) {
    m_mystate = STATE_PATHMOVING_TRACK;
  }
}

void
Ghoul::activate()
{
  if (Editor::is_active())
    return;
}

void
Ghoul::deactivate()
{
  switch (m_mystate) {
    case STATE_TRACKING:
      m_mystate = STATE_IDLE;
      break;
    default:
      break;
  }
}

void
Ghoul::active_update(float dt_sec)
{
  if (Editor::is_active() && get_path() && get_path()->is_valid()) {
    get_walker()->update(dt_sec);
    set_pos(get_walker()->get_pos(m_col.m_bbox.get_size(), m_path_handle));
    return;
  }

  auto player = get_nearest_player();
  if (!player) 
  return;
  Vector p1 = m_col.m_bbox.get_middle();
  Vector p2 = player->get_bbox().get_middle();
  Vector dist = (p2 - p1);
  
  const Rectf& player_bbox = player->get_bbox();
  
  if (player_bbox.get_right() < m_col.m_bbox.get_left()) {
    set_action("left", -1);
  }
  
  if (player_bbox.get_left() > m_col.m_bbox.get_right()) {
    set_action("right", -1);
  }

  switch (m_mystate) {
    case STATE_STOPPED:
      break;

    case STATE_IDLE:
      if (glm::length(dist) <= m_track_range) {
        m_mystate = STATE_TRACKING;
      }
      break;

    case STATE_TRACKING:
      if (glm::length(dist) >= 1) {
        Vector dir_ = glm::normalize(dist);
        m_col.set_movement(dir_ * dt_sec * m_flyspeed);
      } else {
        /* We somehow landed right on top of the player without colliding.
         * Sit tight and avoid a division by zero. */
      }
      break;

    case STATE_PATHMOVING:
    case STATE_PATHMOVING_TRACK:
      if (get_walker() == nullptr)
        return;
      get_walker()->update(dt_sec);
      m_col.set_movement(get_walker()->get_pos(m_col.m_bbox.get_size(), m_path_handle) - get_pos());
      if (m_mystate == STATE_PATHMOVING_TRACK && glm::length(dist) <= m_track_range) {
        m_mystate = STATE_TRACKING;
      }
      break;

    default:
      assert(false);
  }
}

void
Ghoul::goto_node(int node_idx)
{
  get_walker()->goto_node(node_idx);
  if (m_mystate != STATE_PATHMOVING && m_mystate != STATE_PATHMOVING_TRACK) {
    m_mystate = STATE_PATHMOVING;
  }
}

void
Ghoul::start_moving()
{
  get_walker()->start_moving();
}

void
Ghoul::stop_moving()
{
  get_walker()->stop_moving();
}

void
Ghoul::set_state(const std::string& new_state)
{
  if (new_state == "stopped") {
    m_mystate = STATE_STOPPED;
  } else if (new_state == "idle") {
    m_mystate = STATE_IDLE;
  } else if (new_state == "move_path") {
    m_mystate = STATE_PATHMOVING;
    get_walker()->start_moving();
  } else if (new_state == "move_path_track") {
    m_mystate = STATE_PATHMOVING_TRACK;
    get_walker()->start_moving();
  } else if (new_state == "normal") {
    m_mystate = STATE_IDLE;
  } else {
    log_warning << "Can't set unknown state '" << new_state << std::endl;
  }
}

void
Ghoul::move_to(const Vector& pos)
{
  Vector shift = pos - m_col.m_bbox.p1();
  if (get_path()) {
    get_path()->move_by(shift);
  }
  set_pos(pos);
}

std::vector<Direction>
Ghoul::get_allowed_directions() const
{
  return {};
}

/* EOF */
