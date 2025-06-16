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

#include "object/platform.hpp"

#include <simplesquirrel/vm.hpp>

#include "editor/editor.hpp"
#include "object/player.hpp"
#include "supertux/sector.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

Platform::Platform(const ReaderMapping& reader) :
  Platform(reader, "images/objects/flying_platform/flying_platform.sprite")
{
}

Platform::Platform(const ReaderMapping& reader, const std::string& default_sprite) :
  MovingSprite(reader, default_sprite, LAYER_OBJECTS, COLGROUP_STATIC),
  PathObject(),
  m_speed(Vector(0,0)),
  m_movement(Vector(0, 0)),
  m_automatic(false),
  m_player_contact(false),
  m_last_player_contact(false),
  m_starting_node(0)
{
  bool running = true;
  reader.get("running", running);
  if ((get_name().empty()) && (!running)) {
    m_automatic = true;
  }

  reader.get("starting-node", m_starting_node, 0.f);

  init_path(reader, true);
}

void
Platform::finish_construction()
{
  if (!get_path())
  {
    // If no path is given, make a one-node dummy path
    init_path_pos(m_col.m_bbox.p1());
  }

  if (m_starting_node >= static_cast<int>(get_path()->get_nodes().size()))
    m_starting_node = static_cast<int>(get_path()->get_nodes().size()) - 1;

  get_walker()->jump_to_node(m_starting_node);

  m_col.m_bbox.set_pos(m_path_handle.get_pos(m_col.m_bbox.get_size(), get_path()->get_nodes()[m_starting_node].position));
}

ObjectSettings
Platform::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  if (get_path_gameobject())
  {
    result.add_path_ref(_("Path"), *this, get_path_ref(), "path-ref");
    result.add_walk_mode(_("Path Mode"), &get_path()->m_mode, {}, {});
    result.add_bool(_("Adapt Speed"), &get_path()->m_adapt_speed, {}, {});
  }
  result.add_bool(_("Running"), &get_walker()->m_running, "running", true, 0);
  result.add_int(_("Starting Node"), &m_starting_node, "starting-node", 0, 0U);
  result.add_path_handle(_("Handle"), m_path_handle, "handle");

  result.reorder({"running", "name", "path-ref", "starting-node", "sprite", "x", "y"});

  return result;
}

HitResponse
Platform::collision(MovingObject& other, const CollisionHit& )
{
  if (dynamic_cast<Player*>(&other)) {
    m_player_contact = true;
  }
  return FORCE_MOVE;
}

void
Platform::update(float dt_sec)
{
  if (!get_path()) return;
  if (!get_path()->is_valid()) return;

  // check if Platform should automatically pick a destination
  if (m_automatic)
  {
    if (!m_player_contact && !get_walker()->is_running()) {
      // Player doesn't touch platform and Platform is not moving

      // Travel to node nearest to nearest player
      if (auto* player = Sector::get().get_nearest_player(m_col.m_bbox)) {
        int nearest_node_id = get_path()->get_nearest_node_idx(player->get_bbox().p2());
        if (nearest_node_id != -1) {
          goto_node(nearest_node_id);
        }
      }
    }

    if (m_player_contact && !m_last_player_contact && !get_walker()->is_running()) {
      // Player touched platform, didn't touch last frame and Platform is not moving

      // Travel to node farthest from current position
      int farthest_node_id = get_path()->get_farthest_node_idx(get_pos());
      if (farthest_node_id != -1) {
        goto_node(farthest_node_id);
      }
    }

    // Clear player_contact flag set by collision() method
    m_last_player_contact = m_player_contact;
    m_player_contact = false;
  }

  get_walker()->update(dt_sec);
  m_movement = get_walker()->get_pos(m_col.m_bbox.get_size(), m_path_handle) - get_pos();
  m_col.set_movement(m_movement);
  m_col.propagate_movement(m_movement);
  m_speed = m_movement / dt_sec;
}

void
Platform::editor_update()
{
  if (!get_path()) return;
  if (!get_path()->is_valid()) return;

  if (m_starting_node >= static_cast<int>(get_path()->get_nodes().size()))
    m_starting_node = static_cast<int>(get_path()->get_nodes().size()) - 1;

  set_pos(m_path_handle.get_pos(m_col.m_bbox.get_size(), get_path()->get_nodes()[m_starting_node].position));
}

void
Platform::jump_to_node(int node_idx)
{
  set_node(node_idx);
  set_pos(m_path_handle.get_pos(m_col.m_bbox.get_size(), get_path()->get_nodes()[node_idx].position));
}

void
Platform::move_to(const Vector& pos)
{
  Vector shift = pos - m_col.m_bbox.p1();
  if (get_path()) {
    get_path()->move_by(shift);
  }
  set_pos(pos);
}

void
Platform::on_flip(float height)
{
  MovingSprite::on_flip(height);
  PathObject::on_flip();
  FlipLevelTransformer::transform_flip(m_flip);
}

void
Platform::save_state()
{
  MovingSprite::save_state();
  PathObject::save_state();
}

void
Platform::check_state()
{
  MovingSprite::check_state();
  PathObject::check_state();
}


void
Platform::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Platform>("Platform", vm.findClass("MovingSprite"));

  PathObject::register_members(cls);

  // Use Platform's implementation of "set_node".
  cls.addFunc("set_node", &Platform::jump_to_node);
}

/* EOF */
