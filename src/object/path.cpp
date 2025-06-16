//  SuperTux Path
//  Copyright (C) 2005 Philipp <balinor@pnxs.de>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "object/path.hpp"

#include "editor/bezier_marker.hpp"
#include "editor/node_marker.hpp"
#include "math/easing.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "util/log.hpp"

WalkMode
string_to_walk_mode(const std::string& mode_string)
{
  if (mode_string == "oneshot")
    return WalkMode::ONE_SHOT;
  else if (mode_string == "pingpong")
    return WalkMode::PING_PONG;
  else if (mode_string == "circular")
    return WalkMode::CIRCULAR;
  else {
    log_warning << "Unknown path mode '" << mode_string << "' found. Using oneshot instead." << std::endl;
    return WalkMode::ONE_SHOT;
  }
}

std::string
walk_mode_to_string(WalkMode walk_mode)
{
  if (walk_mode == WalkMode::ONE_SHOT)
    return "oneshot";
  else if (walk_mode == WalkMode::PING_PONG)
    return "pingpong";
  else if (walk_mode == WalkMode::CIRCULAR)
    return "circular";
  else {
    log_warning << "Unknown path mode found. Using oneshot instead." << std::endl;
    return "oneshot";
  }
}

Path::Path(PathGameObject& parent) :
  m_parent_gameobject(parent),
  m_nodes(),
  m_mode(WalkMode::CIRCULAR),
  m_adapt_speed()
{
}

Path::Path(const Vector& pos, PathGameObject& parent) :
  m_parent_gameobject(parent),
  m_nodes(),
  m_mode(),
  m_adapt_speed()
{
  Node first_node(this);
  first_node.position = pos;
  first_node.bezier_before = pos;
  first_node.bezier_after = pos;
  first_node.time = 1;
  first_node.speed = 0;
  first_node.easing = EaseNone;
  m_nodes.push_back(first_node);
}

void
Path::read(const ReaderMapping& reader)
{
  m_nodes.clear();
  m_mode = WalkMode::CIRCULAR;
  m_adapt_speed = false;

  auto iter = reader.get_iter();
  while (iter.next()) {
    if (iter.get_key() == "mode") {
      std::string mode_string;
      iter.get(mode_string);
      m_mode = string_to_walk_mode(mode_string);
    } else if (iter.get_key() == "adapt_speed") {
      iter.get(m_adapt_speed);
    } else if (iter.get_key() == "node") {
      ReaderMapping node_mapping = iter.as_mapping();

      // each new node will inherit all values from the last one
      Node node(this);
      node.time = 1;
      node.speed = 0;
      node.easing = EaseNone;
      if (!node_mapping.get("x", node.position.x) ||
          !node_mapping.get("y", node.position.y))
        throw std::runtime_error("Path node without x and y coordinate specified");
      if (!node_mapping.get("bezier_before_x", node.bezier_before.x) ||
          !node_mapping.get("bezier_before_y", node.bezier_before.y))
        node.bezier_before = node.position;
      if (!node_mapping.get("bezier_after_x", node.bezier_after.x) ||
          !node_mapping.get("bezier_after_y", node.bezier_after.y))
        node.bezier_after = node.position;
      node_mapping.get("time", node.time);
      node_mapping.get("speed", node.speed);
      node_mapping.get_custom("easing", node.easing, EasingMode_from_string);

      if (node.time <= 0)
        throw std::runtime_error("Path node with non-positive time");

      m_nodes.push_back(node);
    } else {
      log_warning << "unknown token '" << iter.get_key() << "' in Path nodes list. Ignored." << std::endl;
    }
  }

  if (m_nodes.empty())
    throw std::runtime_error("Path with zero nodes");
}

void
Path::save(Writer& writer)
{
  if (!is_valid()) return;

  if (m_mode != WalkMode::CIRCULAR) {
    writer.write("mode", walk_mode_to_string(m_mode), false);
  }
  writer.write("adapt_speed", m_adapt_speed);

  for (auto& nod : m_nodes) {
    writer.start_list("node");
    writer.write("x", nod.position.x);
    writer.write("y", nod.position.y);

    if (nod.bezier_before.x != nod.position.x || nod.bezier_before.y != nod.position.y)
    {
      writer.write("bezier_before_x", nod.bezier_before.x);
      writer.write("bezier_before_y", nod.bezier_before.y);
    }

    if (nod.bezier_after.x != nod.position.x || nod.bezier_after.y != nod.position.y)
    {
      writer.write("bezier_after_x", nod.bezier_after.x);
      writer.write("bezier_after_y", nod.bezier_after.y);
    }

    if (nod.time != 1.0f) {
      writer.write("time", nod.time);
    }
    if (nod.speed != 0.0f) {
      writer.write("speed", nod.speed);
    }
    if (nod.easing != EaseNone) {
      writer.write("easing", getEasingName(nod.easing));
    }
    writer.end_list("node");
  }
}

Vector
Path::get_base() const
{
  if (m_nodes.empty())
    return Vector(0, 0);

  return m_nodes[0].position;
}

int
Path::get_nearest_node_idx(const Vector& reference_point) const
{
  int nearest_node_id = -1;
  float nearest_node_dist = 0;
  int id = 0;
  for (std::vector<Node>::const_iterator i = m_nodes.begin(); i != m_nodes.end(); ++i, ++id) {
    float dist = glm::distance(i->position, reference_point);
    if ((nearest_node_id == -1) || (dist < nearest_node_dist)) {
      nearest_node_id = id;
      nearest_node_dist = dist;
    }
  }
  return nearest_node_id;
}

int
Path::get_farthest_node_idx(const Vector& reference_point) const
{
  int farthest_node_id = -1;
  float farthest_node_dist = 0;
  int id = 0;
  for (std::vector<Node>::const_iterator i = m_nodes.begin(); i != m_nodes.end(); ++i, ++id)
  {
    float dist = glm::distance(i->position, reference_point);
    if ((farthest_node_id == -1) || (dist > farthest_node_dist)) {
      farthest_node_id = id;
      farthest_node_dist = dist;
    }
  }
  return farthest_node_id;
}

void
Path::move_by(const Vector& shift)
{
  for (auto& nod : m_nodes) {
    nod.position += shift;
    nod.bezier_before += shift;
    nod.bezier_after += shift;
  }
}

void
Path::edit_path()
{
  int id = 0;
  for (auto i = m_nodes.begin(); i != m_nodes.end(); ++i) {
    auto& before = Sector::get().add<BezierMarker>(&(*i), &(i->bezier_before));
    auto& after = Sector::get().add<BezierMarker>(&(*i), &(i->bezier_after));
    auto& nm = Sector::get().add<NodeMarker>(i, id, before.get_uid(), after.get_uid());
    before.set_parent(nm.get_uid());
    after.set_parent(nm.get_uid());
    id++;
  }
}

bool
Path::is_valid() const
{
  return !m_nodes.empty();
}

void
Path::on_flip(float height)
{
  for (auto& node : m_nodes) {
    node.position.y = height - node.position.y;
    node.bezier_before.y = height - node.bezier_before.y;
    node.bezier_after.y = height - node.bezier_after.y;
  }
}

/* EOF */
