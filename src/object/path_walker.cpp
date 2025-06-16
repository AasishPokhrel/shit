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

#include "object/path_walker.hpp"

#include <math.h>
#include <assert.h>

#include "editor/editor.hpp"
#include "editor/object_option.hpp"
#include "math/bezier.hpp"
#include "math/random.hpp"
#include "object/path_gameobject.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/sector.hpp"
#include "util/gettext.hpp"
#include "math/easing.hpp"

Vector
PathWalker::Handle::get_pos(const Sizef& size, const Vector& pos) const
{
  return pos - Vector(size.width * m_scalar_pos.x, size.height * m_scalar_pos.y) - m_pixel_offset;
}

PathWalker::PathWalker(UID path_uid, bool running_) :
  m_path_uid(path_uid),
  m_running(running_),
  m_current_node_nr(0),
  m_next_node_nr(),
  m_stop_at_node_nr(m_running ? -1 : 0),
  m_node_time(0),
  m_node_mult(),
  m_walking_speed(1.0)
{
  Path* path = get_path();
  if (!path) return;
  if (!path->is_valid()) return;

  m_next_node_nr = path->m_nodes.size() > 1 ? 1 : 0;
  m_node_mult = 1 / path->m_nodes[0].time;
}

PathWalker::~PathWalker()
{
}

Path*
PathWalker::get_path() const
{
  if (!d_sector) return nullptr;

  auto path_gameobject = d_sector->get_object_by_uid<PathGameObject>(m_path_uid);
  if (!path_gameobject)
  {
    return nullptr;
  }
  else
  {
    return &path_gameobject->get_path();
  }
}

void
PathWalker::update(float dt_sec)
{
  Path* path = get_path();
  if (!path) return;
  if (!path->is_valid()) return;
  if (Editor::is_active()) return;
  if (!m_running) return;

  float delta = fabsf(m_walking_speed) * dt_sec;

  while (m_node_time + delta * m_node_mult >= 1) {
    delta -= (1 - m_node_time) / m_node_mult;

    if (m_walking_speed > 0) {
      advance_node();
    } else if (m_walking_speed < 0) {
      goback_node();
    }

    auto current_node = & (path->m_nodes[m_current_node_nr]);
    m_node_time = 0;
    if (m_walking_speed > 0) {
      m_node_mult = 1 / current_node->time;
    } else {
      m_node_mult = 1 / path->m_nodes[m_next_node_nr].time;
    }
  }

  m_node_time += delta * m_node_mult;
}

Vector
PathWalker::get_pos(const Sizef& object_size, const Handle& handle) const
{
  Path* path = get_path();
  if (!path) return Vector(0, 0);
  if (!path->is_valid()) return Vector(0, 0);
  if (Editor::is_active()) return path->m_nodes.begin()->position;
  if (!m_running) return path->m_nodes[m_current_node_nr].position;

  const Path::Node* current_node = &(path->m_nodes[m_current_node_nr]);
  const Path::Node* next_node = & (path->m_nodes[m_next_node_nr]);

  easing easeFunc = m_walking_speed > 0 ?
                          getEasingByName(current_node->easing) :
                          getEasingByName(get_reverse_easing(next_node->easing));

  float progress = static_cast<float>(easeFunc(static_cast<double>(m_node_time)));

  Vector p1 = current_node->position,
         p2 = m_walking_speed > 0 ? current_node->bezier_after : current_node->bezier_before,
         p3 = m_walking_speed > 0 ? next_node->bezier_before : next_node->bezier_after,
         p4 = next_node->position;

  Vector position = path->m_adapt_speed ?
                          Bezier::get_point(p1, p2, p3, p4, progress) :
                          Bezier::get_point_by_length(p1, p2, p3, p4, progress);

  return handle.get_pos(object_size, position);
}

void
PathWalker::goto_node(int node_idx)
{
  const Path* path = get_path();
  if (!path) return;

  if (node_idx == m_stop_at_node_nr) return;
  m_running = true;
  m_stop_at_node_nr = node_idx;
}

void
PathWalker::jump_to_node(int node_idx, bool instantaneous)
{
  Path* path = get_path();
  if (!path) return;

  if (node_idx >= static_cast<int>(path->get_nodes().size())) return;
  m_next_node_nr = static_cast<size_t>(node_idx);
  if (instantaneous || m_walking_speed == 0) {
    m_current_node_nr = m_next_node_nr;
  }

  if (m_walking_speed > 0) {
    advance_node();
  } else if (m_walking_speed < 0) {
    goback_node();
  }
  m_node_time = 0.f;
}

void
PathWalker::start_moving()
{
  m_running = true;
  m_stop_at_node_nr = -1;
}

void
PathWalker::stop_moving()
{
  m_stop_at_node_nr = static_cast<int>(m_next_node_nr);
}

void
PathWalker::advance_node()
{
  Path* path = get_path();
  if (!path) return;
  if (!path->is_valid()) return;

  auto node_count = path->m_nodes.size();

  m_current_node_nr = m_next_node_nr;
  if (static_cast<int>(m_current_node_nr) == m_stop_at_node_nr) m_running = false;

  if (m_next_node_nr + 1 < node_count) {
    m_next_node_nr++;
    return;
  }

  switch (path->m_mode) {
    case WalkMode::ONE_SHOT:
      m_next_node_nr = node_count - 1;
      m_walking_speed = 0;
      return;

    case WalkMode::PING_PONG:
      m_walking_speed = -m_walking_speed;
      m_next_node_nr = node_count > 1 ? node_count - 2 : 0;
      return;

    case WalkMode::CIRCULAR:
      m_next_node_nr = 0;
      return;
  }

  // we shouldn't get here
  assert(false);
  m_next_node_nr = node_count - 1;
  m_walking_speed = 0;
}

void
PathWalker::goback_node()
{
  Path* path = get_path();
  if (!path) return;
  if (!path->is_valid()) return;

  m_current_node_nr = m_next_node_nr;

  if (m_next_node_nr > 0) {
    m_next_node_nr--;
    return;
  }

  switch (path->m_mode) {
    case WalkMode::PING_PONG:
      m_walking_speed = -m_walking_speed;
      m_next_node_nr = path->m_nodes.size() > 1 ? 1 : 0;
      return;
    default:
      break;
  }

  assert(false);
  m_next_node_nr = 0;
  m_walking_speed = 0;
}

/* EOF */
