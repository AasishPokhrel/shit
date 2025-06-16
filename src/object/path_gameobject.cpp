//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "object/path_gameobject.hpp"

#include <optional>

#include "editor/node_marker.hpp"
#include "object/path.hpp"
#include "object/path_object.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/debug.hpp"
#include "supertux/level.hpp"
#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/unique_name.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

namespace {

PathStyle PathStyle_from_string(const std::string& text)
{
  if (text == "none") {
    return PathStyle::NONE;
  } else if (text == "solid") {
    return PathStyle::SOLID;
  } else {
    log_warning << "unknown PathStyle: " << text << std::endl;
    return PathStyle::NONE;
  }
}

} // namespace

PathGameObject::PathGameObject() :
  m_path(new Path(*this)),
  m_style(PathStyle::NONE),
  m_edge_sprite(),
  m_node_sprite()
{
  m_track_undo = false;

  m_name = make_unique_name("path", this);
}

PathGameObject::PathGameObject(const Vector& pos) :
  m_path(new Path(pos, *this)),
  m_style(PathStyle::NONE),
  m_edge_sprite(),
  m_node_sprite()
{
  m_track_undo = false;

  m_name = make_unique_name("path", this);
}

PathGameObject::PathGameObject(const ReaderMapping& mapping, bool backward_compatibility_hack) :
  GameObject(mapping),
  m_path(new Path(*this)),
  m_style(PathStyle::NONE),
  m_edge_sprite(),
  m_node_sprite()
{
  if (backward_compatibility_hack)
  {
    m_path->read(mapping);
  }
  else
  {
    std::optional<ReaderMapping> path_mapping;
    if (mapping.get("path", path_mapping))
    {
      m_path->read(*path_mapping);
    }
  }

  mapping.get_custom("style", m_style, PathStyle_from_string);

  if (m_style == PathStyle::SOLID)
  {
    m_edge_sprite = SpriteManager::current()->create("images/objects/path/edge.sprite");
    m_node_sprite = SpriteManager::current()->create("images/objects/path/node.sprite");
  }

  if (m_name.empty())
    regenerate_name();
}

PathGameObject::~PathGameObject()
{
}

void
PathGameObject::update(float dt_sec)
{
}

void
PathGameObject::draw(DrawingContext& context)
{
  if (m_style == PathStyle::SOLID)
  {
    std::optional<Vector> previous_node;

    for (const auto& node : m_path->get_nodes())
    {
      if (previous_node)
      {
        const Vector p1 = *previous_node;
        const Vector p2 = node.position;
        const Vector diff = (p2 - p1);
        const float length = glm::length(diff);
        const Vector unit = glm::normalize(diff);
        float dot_distance = 16.0f;

        // Recalculate the dot distance to evenly spread across the
        // whole edge
        dot_distance = length / floorf(length / dot_distance);

        for (float i = dot_distance; i < length; i += dot_distance) // NOLINT
        {
          Vector dot_pos = p1 + unit * i;
          m_edge_sprite->draw(context.color(), Vector(dot_pos), LAYER_OBJECTS - 1);
        }
      }

      m_node_sprite->draw(context.color(), node.position, LAYER_OBJECTS - 1);

      previous_node = node.position;
    }
  }

  if (g_debug.show_collision_rects)
  {
    const Color node_color = Color::BLUE;
    const Color edge_color = Color::MAGENTA;

    std::optional<Vector> previous_node;
    for (const auto& node : m_path->get_nodes())
    {
      if (previous_node)
      {
        context.color().draw_line(*previous_node, node.position, edge_color, LAYER_OBJECTS - 2);
      }

      context.color().draw_filled_rect(Rectf::from_center(node.position, Sizef(16.0f, 16.0f)),
                                       node_color, LAYER_OBJECTS - 1);

      previous_node = node.position;
    }
  }
}

ObjectSettings
PathGameObject::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_path(_("Path"), m_path.get(), "path");

  return result;
}

void
PathGameObject::editor_select()
{
  log_fatal << "PathGameObject::selected" << std::endl;
}

void
PathGameObject::editor_deselect()
{
  log_fatal << "PathGameObject::deselected" << std::endl;
}

void
PathGameObject::remove_me()
{
  if (Sector::current())
  {
    auto handles = Sector::get().get_objects_by_type<NodeMarker>();

    for (auto& handle : handles)
      handle.remove_me(); // Removing a node handle also removes its bezier handles
  }

  GameObject::remove_me();
}

void
PathGameObject::copy_into(PathGameObject& other)
{
  other.get_path().m_nodes = get_path().m_nodes;
}

bool
PathGameObject::is_saveable() const
{
  if (!Sector::current())
    return false;

  for (const auto& sector : Level::current()->get_sectors())
  {
    for (const auto& path_obj : sector->get_objects_by_type<PathObject>())
      if (path_obj.get_path_gameobject() == this)
        return true;
  }

  return false;
}

void
PathGameObject::on_flip(float height)
{
  m_path->on_flip(height);
}

void
PathGameObject::regenerate_name()
{
  set_name(make_unique_name("path", this));
}

/* EOF */
