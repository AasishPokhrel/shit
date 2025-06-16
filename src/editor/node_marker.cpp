//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#include "editor/node_marker.hpp"

#include "editor/editor.hpp"
#include "math/bezier.hpp"
#include "math/easing.hpp"
#include "supertux/sector.hpp"

NodeMarker::NodeMarker(std::vector<Path::Node>::iterator node_iterator, size_t id_, UID before, UID after) :
  m_path(&node_iterator->get_parent()),
  m_bezier_before(before),
  m_bezier_after(after),
  m_node(node_iterator),
  m_id(id_)
{
  set_pos(m_node->position - Vector(8, 8));
}

void
NodeMarker::update_iterator()
{
  if (m_id >= m_path->m_nodes.size()) {
    remove_me();
  } else {
    m_node = m_path->m_nodes.begin() + m_id;

    BezierMarker* before = Sector::current()->get_object_by_uid<BezierMarker>(m_bezier_before);
    BezierMarker* after = Sector::current()->get_object_by_uid<BezierMarker>(m_bezier_after);

    if (before)
      before->update_iterator(&(*m_node), &(m_node->bezier_before));
    if (after)
      after->update_iterator(&(*m_node), &(m_node->bezier_after));
  }
}

void
NodeMarker::remove_me()
{
  BezierMarker* before = Sector::current()->get_object_by_uid<BezierMarker>(m_bezier_before);
  BezierMarker* after = Sector::current()->get_object_by_uid<BezierMarker>(m_bezier_after);

  if (before)
    before->remove_me();
  if (after)
    after->remove_me();

  MarkerObject::remove_me();
}

Vector
NodeMarker::get_point_vector() const
{
  std::vector<Path::Node>::const_iterator next = next_node();
  if (next == m_path->m_nodes.end()) {
    return Vector(0,0);
  } else {
    Vector p = Bezier::get_point(m_node->position,
                                 m_node->bezier_after,
                                 next->bezier_before,
                                 next->position,
                                 .01f);

    return p - m_node->position;
  }
}

Vector
NodeMarker::get_offset() const
{
  return Vector(8, 8);
}

void
NodeMarker::move_to(const Vector& pos)
{
  BezierMarker* before = Sector::current()->get_object_by_uid<BezierMarker>(m_bezier_before);
  BezierMarker* after = Sector::current()->get_object_by_uid<BezierMarker>(m_bezier_after);

  if (before)
    before->move_to(pos + (before->get_pos() - get_pos()));
  if (after)
    after->move_to(pos + (after->get_pos() - get_pos()));

  MovingObject::move_to(pos);
  m_node->position = m_col.m_bbox.get_middle();
  update_node_times();
}

void
NodeMarker::editor_delete()
{
  if (m_path->m_nodes.size() <= 1)
  {
    return;
  }
  auto prev = prev_node();
  auto next = next_node();
  update_node_time(prev, next);
  m_path->m_nodes.erase(m_node);
  Editor::current()->update_node_iterators();
}

ObjectSettings
NodeMarker::get_settings()
{
  ObjectSettings result(_("Path Node"));
  result.add_label(_("Press CTRL to move Bezier handles"));
  result.add_float(_("Time"), &(m_node->time));
  result.add_float(_("Speed"), &(m_node->speed));

  result.add_enum(_("Easing"), reinterpret_cast<int*>(&(m_node->easing)),
                  {
                    _("No easing"),
                    _("Quad in"), _("Quad out"), _("Quad in/out"),
                    _("Cubic in"), _("Cubic out"), _("Cubic in/out"),
                    _("Quart in"), _("Quart out"), _("Quart in/out"),
                    _("Quint in"), _("Quint out"), _("Quint in/out"),
                    _("Sine in"), _("Sine out"), _("Sine in/out"),
                    _("Circular in"), _("Circular out"), _("Circular in/out"),
                    _("Exponential in"), _("Exponential out"), _("Exponential in/out"),
                    _("Elastic in"), _("Elastic out"), _("Elastic in/out"),
                    _("Back in"), _("Back out"), _("Back in/out"),
                    _("Bounce in"), _("Bounce out"), _("Bounce in/out")
                  },
                  {
                    "EaseNone",
                    "EaseQuadIn", "EaseQuadOut", "EaseQuadInOut",
                    "EaseCubicIn", "EaseCubicOut", "EaseCubicInOut",
                    "EaseQuartIn", "EaseQuartOut", "EaseQuartInOut",
                    "EaseQuintIn", "EaseQuintOut", "EaseQuintInOut",
                    "EaseSineIn", "EaseSineOut", "EaseSineInOut",
                    "EaseCircularIn", "EaseCircularOut", "EaseCircularInOut",
                    "EaseExponentialIn", "EaseExponentialOut", "EaseExponentialInOut",
                    "EaseElasticIn", "EaseElasticOut", "EaseElasticInOut",
                    "EaseBackIn", "EaseBackOut", "EaseBackInOut",
                    "EaseBounceIn", "EaseBounceOut", "EaseBounceInOut"
                  },
                  0, "easing");
  return result;
}

void
NodeMarker::editor_update()
{
  set_pos(m_node->position - Vector(8, 8));
  update_node_time(m_node, next_node());
}

std::vector<Path::Node>::iterator NodeMarker::prev_node() {
  std::vector<Path::Node>::iterator node = m_node;
  if (node == m_path->m_nodes.begin()) {
    if (m_path->m_mode == WalkMode::CIRCULAR) {
      node = m_path->m_nodes.end();
    } else {
      return m_path->m_nodes.end();
    }
  }
  --node;
  return node;
}

std::vector<Path::Node>::const_iterator NodeMarker::next_node() const {
  std::vector<Path::Node>::const_iterator node = m_node + 1;
  if (node == m_path->m_nodes.end()) {
    if (m_path->m_mode == WalkMode::CIRCULAR) {
      node = m_path->m_nodes.begin();
    }
  }
  return node;
}

void NodeMarker::update_node_times() {
  update_node_time(prev_node(), m_node);
  update_node_time(m_node, next_node());
}

void NodeMarker::update_node_time(std::vector<Path::Node>::iterator current, std::vector<Path::Node>::const_iterator next) {
  if (current == m_path->m_nodes.end() || next == m_path->m_nodes.end() || current->speed <= 0) {
    return;  // Nothing to do.
  }

  float delta = Bezier::get_length(current->position,
                                    current->bezier_after,
                                    next->bezier_before,
                                    next->position);
  if (delta > 0) {
    current->time = delta / current->speed;
  }
}

void
NodeMarker::move_other_marker(UID marker, Vector position)
{
  assert(marker == m_bezier_before || marker == m_bezier_after);

  auto bm = Sector::current()->get_object_by_uid<BezierMarker>(
                (marker == m_bezier_before) ? m_bezier_after : m_bezier_before);

  if (bm)
    bm->move_to(position);
}

void
NodeMarker::save_state()
{
  m_path->get_gameobject().save_state();
}

void
NodeMarker::check_state()
{
  m_path->get_gameobject().check_state();
}

/* EOF */
