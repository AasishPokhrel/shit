//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "editor/overlay_widget.hpp"

#include <fmt/format.h>

#include "editor/editor.hpp"
#include "editor/node_marker.hpp"
#include "editor/object_menu.hpp"
#include "editor/object_info.hpp"
#include "editor/tile_selection.hpp"
#include "editor/tip.hpp"
#include "gui/menu.hpp"
#include "gui/menu_manager.hpp"
#include "math/bezier.hpp"
#include "object/camera.hpp"
#include "object/path_gameobject.hpp"
#include "object/tilemap.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/autotile.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/resources.hpp"
#include "supertux/sector.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"
#include "worldmap/worldmap_object.hpp"

namespace {

const int snap_grid_sizes[4] = {4, 8, 16, 32};

const int MAX_FILL_STACK_SIZE = 1000000;

bool is_position_inside_tilemap(const TileMap* tilemap, const Vector& pos)
{
  return pos.x >= 0 && pos.y >= 0 &&
         pos.x < static_cast<float>(tilemap->get_width()) &&
         pos.y < static_cast<float>(tilemap->get_height());
}

} // namespace

bool EditorOverlayWidget::action_pressed = false;
bool EditorOverlayWidget::alt_pressed = false;

EditorOverlayWidget::EditorOverlayWidget(Editor& editor) :
  m_editor(editor),
  m_hovered_tile(0, 0),
  m_hovered_tile_prev(0, 0),
  m_last_hovered_tile(0, 0),
  m_sector_pos(0, 0),
  m_mouse_pos(0, 0),
  m_previous_mouse_pos(0, 0),
  m_time_prev_put_tile(std::chrono::steady_clock::now()),
  m_dragging(false),
  m_dragging_right(false),
  m_scrolling(false),
  m_drag_start(0, 0),
  m_dragged_object(nullptr),
  m_hovered_object(nullptr),
  m_selected_object(nullptr),
  m_edited_path(nullptr),
  m_last_node_marker(nullptr),
  m_available_autotilesets(),
  m_current_autotileset(0),
  m_object_tip(new Tip()),
  m_obj_mouse_desync(0, 0),
  m_rectangle_preview(new TileSelection()),
  m_warning_timer(),
  m_warning_text(),
  m_selection_warning(false),
  m_autotile_mode(g_config->editor_autotile_mode)
{
}

EditorOverlayWidget::~EditorOverlayWidget()
{
}

void
EditorOverlayWidget::update(float dt_sec)
{
  if (m_hovered_object && !m_hovered_object->is_valid())
  {
    m_hovered_object = nullptr;
    m_object_tip->set_visible(false);
  }

  if (m_selected_object && !m_selected_object->is_valid())
  {
    delete_markers();
  }
}

void
EditorOverlayWidget::on_level_change()
{
  m_dragged_object = nullptr;
  m_selected_object = nullptr;
  m_edited_path = nullptr;
  m_last_node_marker = nullptr;
  m_hovered_object = nullptr;
}

void
EditorOverlayWidget::delete_markers()
{
  auto* sector = m_editor.get_sector();

  if (m_selected_object && m_selected_object->is_valid())
    m_selected_object->editor_deselect();

  sector->flush_game_objects(); // Flush any markers queued to be added
  for (auto& marker : sector->get_objects_by_type<MarkerObject>())
  {
    marker.remove_me();
  }

  m_selected_object = nullptr;
  m_edited_path = nullptr;
  m_last_node_marker = nullptr;
}

Rectf
EditorOverlayWidget::drag_rect() const
{
  float start_x, start_y, end_x, end_y;

  if (m_drag_start.x < m_sector_pos.x)
  {
    start_x = m_drag_start.x;
    end_x = m_sector_pos.x;
  }
  else
  {
    start_x = m_sector_pos.x;
    end_x = m_drag_start.x;
  }

  if (m_drag_start.y < m_sector_pos.y)
  {
    start_y = m_drag_start.y;
    end_y = m_sector_pos.y;
  }
  else
  {
    start_y = m_sector_pos.y;
    end_y = m_drag_start.y;
  }

  return Rectf(start_x, start_y, end_x, end_y);
}

void
EditorOverlayWidget::input_tile(const Vector& pos, uint32_t tile)
{
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap || !is_position_inside_tilemap(tilemap, pos)) return;

  tilemap->save_state();
  tilemap->change(static_cast<int>(pos.x), static_cast<int>(pos.y), tile);
}

void
EditorOverlayWidget::input_autotile(const Vector& pos, uint32_t tile)
{
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap || !is_position_inside_tilemap(tilemap, pos)) return;

  tilemap->save_state();
  tilemap->autotile(pos, tile, get_current_autotileset());
}

void
EditorOverlayWidget::input_autotile_erase(const Vector& pos)
{
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap || !is_position_inside_tilemap(tilemap, pos)) return;

  tilemap->save_state();
  tilemap->autotile_erase(pos, get_current_autotileset());
}

void
EditorOverlayWidget::put_tiles(const Vector& target_tile, TileSelection* tiles)
{
  m_editor.get_selected_tilemap()->save_state();

  Vector add_tile(0.0f, 0.0f);
  for (add_tile.x = static_cast<float>(tiles->m_width) - 1.0f; add_tile.x >= 0.0f; add_tile.x--)
  {
    for (add_tile.y = static_cast<float>(tiles->m_height) - 1.0f; add_tile.y >= 0; add_tile.y--)
    {
      const uint32_t tile = tiles->pos(static_cast<int>(add_tile.x), static_cast<int>(add_tile.y));

      if (m_autotile_mode)
      {
        auto autotileset = get_current_autotileset();
        if (autotileset)
        {
          if (tile == 0)
            input_autotile_erase(target_tile + add_tile);
          else
            input_autotile(target_tile + add_tile, tile);
          continue;
        }
      }

      input_tile(target_tile + add_tile, tile);
    } // for tile y
  } // for tile x
}

namespace {
  // Get integer positions of cartesian grid cells which intersect the line
  // segment from pos1 to pos2 (similarly to a line drawing algorithm)
  std::vector<Vector> rasterize_line_segment(Vector pos1, Vector pos2)
  {
    if (pos1 == pos2)
    {
      return std::vector<Vector>{ pos1 };
    }

    // An integer position (x, y) contains all floating point vectors in
    // [x, x+1) x [y, y+1)
    std::vector<Vector> positions;
    Vector diff = pos2 - pos1;
    if (fabsf(diff.x) > fabsf(diff.y))
    {
      // Go along X, from left to right
      if (diff.x < 0)
      {
        Vector tmp = pos1;
        pos1 = pos2;
        pos2 = tmp;
      }
      positions.emplace_back(pos1);
      float y_prev = pos1.y;
      float y_step = diff.y / diff.x;
      // The x coordinate of the first vertical grid line right of pos1
      float x_first_gridline = floorf(pos1.x + 1.0f);
      for (float x = x_first_gridline; x < pos2.x; ++x)
      {
        // The y coordinate where our line intersects the vertical grid line
        float y = pos1.y + (x - pos1.x) * y_step;
        if (floorf(y) != floorf(y_prev))
        {
          // The current position is one horizontal grid line higher than
          // the previous one,
          // so add the position left to the current vertical grid line
          positions.emplace_back(Vector(x - 0.5f, y));
          y_prev = y;
        }
        // Add the position right to the current vertical grid line
        positions.emplace_back(Vector(x + 0.5f, y));
      }
      if (x_first_gridline > pos2.x && floorf(pos2.y) != floorf(pos1.y))
      {
        // Special case: a single horizontal grid line is crossed with an acute
        // angle but no vertical grid line, so the for loop was skipped
        positions.emplace_back(pos2);
      }
    }
    else
    {
      // Go along Y, from top to bottom
      if (diff.y < 0)
      {
        Vector tmp = pos1;
        pos1 = pos2;
        pos2 = tmp;
      }
      positions.emplace_back(pos1);
      float x_prev = pos1.x;
      float x_step = diff.x / diff.y;
      float y_first_gridline = floorf(pos1.y + 1.0f);
      for (float y = y_first_gridline; y < pos2.y; ++y)
      {
        float x = pos1.x + (y - pos1.y) * x_step;
        if (floorf(x) != floorf(x_prev))
        {
          positions.emplace_back(Vector(x, y - 0.5f));
          x_prev = x;
        }
        positions.emplace_back(Vector(x, y + 0.5f));
      }
      if (y_first_gridline > pos2.y && floorf(pos2.x) != floorf(pos1.x))
      {
        positions.emplace_back(pos2);
      }
    }
    return positions;
  }
}  // namespace

void
EditorOverlayWidget::put_next_tiles()
{
  auto time_now = std::chrono::steady_clock::now();
  int expired_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(time_now - m_time_prev_put_tile).count());
  m_time_prev_put_tile = time_now;
  if (expired_ms > 70)
  {
    // Avoid drawing lines when the user has hold the left mouse button for some
    // time while not putting a tile
    put_tiles(m_hovered_tile, m_editor.get_tiles());
    m_hovered_tile_prev = m_hovered_tile;
    return;
  }
  // Interpolate on a sub-grid with twice width and height because autotiling
  // needs to know the closest corner
  for (const Vector &pos : rasterize_line_segment(m_hovered_tile_prev * 2.0f,
      m_hovered_tile * 2.0f))
  {
    put_tiles(pos * 0.5f, m_editor.get_tiles());
  }
  m_hovered_tile_prev = m_hovered_tile;
}

void
EditorOverlayWidget::preview_rectangle()
{
  Rectf dr = drag_rect();
  dr.set_p1(glm::floor(sp_to_tp(dr.p1())));
  dr.set_p2(glm::floor(sp_to_tp(dr.p2())));
  bool sgn_x = m_drag_start.x < m_sector_pos.x;
  bool sgn_y = m_drag_start.y < m_sector_pos.y;

  m_rectangle_preview->m_tiles.clear();
  m_rectangle_preview->m_width = static_cast<int>(dr.get_width()) + 1;
  m_rectangle_preview->m_height = static_cast<int>(dr.get_height()) + 1;
  int y_ = sgn_y ? 0 : static_cast<int>(-dr.get_height());
  for (int y = static_cast<int>(dr.get_top()); y <= static_cast<int>(dr.get_bottom()); y++, y_++)
  {
    int x_ = sgn_x ? 0 : static_cast<int>(-dr.get_width());
    for (int x = static_cast<int>(dr.get_left()); x <= static_cast<int>(dr.get_right()); x++, x_++)
    {
      m_rectangle_preview->m_tiles.push_back(m_editor.get_tiles()->pos(x_, y_));
    }
  }
}

bool
EditorOverlayWidget::check_tiles_for_fill(uint32_t replace_tile,
                                          uint32_t target_tile,
                                          uint32_t third_tile) const
{
  if (m_autotile_mode)
  {
    return m_editor.get_tileset()->has_mutual_autotileset(replace_tile, target_tile) &&
          !m_editor.get_tileset()->has_mutual_autotileset(replace_tile, third_tile);
  }
  else
  {
    return replace_tile == target_tile && replace_tile != third_tile;
  }
}

void
EditorOverlayWidget::fill()
{
  auto tiles = m_editor.get_tiles();
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap) return;

  // The tile that is going to be replaced:
  uint32_t replace_tile = tilemap->get_tile_id(m_hovered_tile);

  if (replace_tile == tiles->pos(0, 0))
  {
    // Replacing by the same tiles shouldn't do anything.
    return;
  }

  std::vector<Vector> pos_stack = { m_hovered_tile };

  // Passing recursively through all tiles to be replaced...
  while (pos_stack.size())
  {
    if (pos_stack.size() > MAX_FILL_STACK_SIZE)
    {
      log_warning << "More than 1'000'000 tiles in stack to fill, STOP" << std::endl;
      return;
    }

    Vector pos = pos_stack.back();
    Vector tpos = pos - m_hovered_tile;

    // Tests for being inside tilemap:
    if (!is_position_inside_tilemap(tilemap, pos))
    {
      pos_stack.pop_back();
      continue;
    }

    // Autotile will happen later, so that directional filling works properly
    input_tile(pos, tiles->pos(static_cast<int>(tpos.x), static_cast<int>(tpos.y)));

    Vector pos_(0.0f, 0.0f);

    // Going left...
    pos_ = pos + Vector(-1, 0);
    if (pos_.x >= 0)
    {
      if (check_tiles_for_fill(replace_tile,
          tilemap->get_tile_id(pos_),
          tiles->pos(static_cast<int>(tpos.x - 1), static_cast<int>(tpos.y))))
      {
        pos_stack.push_back( pos_ );
        continue;
      }
    }

    // Going right...
    pos_ = pos + Vector(1, 0);
    if (pos_.x < static_cast<float>(tilemap->get_width()))
    {
      if (check_tiles_for_fill(replace_tile,
          tilemap->get_tile_id(pos_),
          tiles->pos(static_cast<int>(tpos.x + 1), static_cast<int>(tpos.y))))
      {
        pos_stack.push_back( pos_ );
        continue;
      }
    }

    // Going up...
    pos_ = pos + Vector(0, -1);
    if (pos_.y >= 0)
    {
      if (check_tiles_for_fill(replace_tile,
          tilemap->get_tile_id(pos_),
          tiles->pos(static_cast<int>(tpos.x), static_cast<int>(tpos.y - 1))))
      {
        pos_stack.push_back( pos_ );
        continue;
      }
    }

    // Going down...
    pos_ = pos + Vector(0, 1);
    if (pos_.y < static_cast<float>(tilemap->get_height()))
    {
      if (check_tiles_for_fill(replace_tile,
          tilemap->get_tile_id(pos_),
          tiles->pos(static_cast<int>(tpos.x), static_cast<int>(tpos.y + 1))))
      {
        pos_stack.push_back( pos_ );
        continue;
      }
    }

    // Autotile happens after directional detection (because of borders; see snow tileset)
    if (m_autotile_mode)
      input_autotile(pos, tiles->pos(static_cast<int>(tpos.x), static_cast<int>(tpos.y)));

    // When tiles on each side are already filled or occupied by another tiles, it ends.
    pos_stack.pop_back();
  }
}

void
EditorOverlayWidget::replace()
{
  auto tilemap = m_editor.get_selected_tilemap();
  uint32_t replace_tile = tilemap->get_tile_id(m_hovered_tile);

  // Don't do anything if the old and new tiles are the same tile.
  if (m_editor.get_tiles()->m_width == 1 && m_editor.get_tiles()->m_height == 1 && replace_tile == m_editor.get_tiles()->pos(0, 0)) return;

  tilemap->save_state();
  for (int x = 0; x < tilemap->get_width(); ++x)
  {
    for (int y = 0; y < tilemap->get_height(); ++y)
    {
      if (tilemap->get_tile_id(x, y) == replace_tile)
      {
        tilemap->change(x, y, m_editor.get_tiles()->pos(
          (x - static_cast<int>(m_hovered_tile.x)) % m_editor.get_tiles()->m_width,
          (y - static_cast<int>(m_hovered_tile.y)) % m_editor.get_tiles()->m_height)
        );
      }
    }
  }
}

void
EditorOverlayWidget::hover_object()
{
  m_object_tip->set_visible(false);
  m_hovered_object = nullptr;

  BezierMarker* marker_hovered_without_ctrl = nullptr;

  bool cache_is_marker = false;
  int cache_layer = INT_MIN;

  for (auto& moving_object : m_editor.get_sector()->get_objects_by_type<MovingObject>())
  {
    const Rectf& bbox = moving_object.get_bbox();
    if (bbox.contains(m_sector_pos))
    {
      if (&moving_object != m_hovered_object)
      {

        // Ignore BezierMarkers if ctrl isn't pressed... (1/2)
        auto* bezier_marker = dynamic_cast<BezierMarker*>(&moving_object);
        if (bezier_marker)
        {
          if (!m_editor.m_ctrl_pressed)
          {
            marker_hovered_without_ctrl = bezier_marker;
            continue;
          }
          else
          {
            cache_is_marker = true;
            cache_layer = 2147483647;
            m_hovered_object = &moving_object;
          }
        }

        // Pick objects in this priority:
        //   1. Markers
        //   2. Objects with a higher layer ID
        //   3. If many objects are on the highest layer, pick the last created one
        //      (Which will be the one rendererd on top)

        bool is_marker = dynamic_cast<MarkerObject*>(&moving_object);
        // The "=" part of ">=" ensures that for equal layer, the last object is picked; don't remove the "="!
        if ((is_marker && !cache_is_marker) || moving_object.get_layer() >= cache_layer)
        {
          cache_is_marker = is_marker;
          cache_layer = moving_object.get_layer();
          m_hovered_object = &moving_object;
        }
      }
    }
  }

  if (m_hovered_object && m_hovered_object->has_settings() && !m_editor.has_active_toolbox_tip()) {
    m_object_tip->set_info_for_object(*m_hovered_object);
  }

  // (2/2) ...but select them anyways if they weren't hovering a node marker
  if (marker_hovered_without_ctrl && !m_hovered_object)
  {
    m_hovered_object = marker_hovered_without_ctrl;
    // TODO: Temporarily disabled during ongoing discussion
    //m_object_tip = std::make_unique<Tip>(_("Press ALT to make Bezier handles continuous"));
    return;
  }
}

void
EditorOverlayWidget::edit_path(PathGameObject* path, GameObject* new_marked_object)
{
  if (!path) return;
  delete_markers();

  if (!path->is_valid())
  {
    m_edited_path = nullptr;
    return;
  }
  m_edited_path = path;
  m_edited_path->get_path().edit_path();
  if (new_marked_object) {
    m_selected_object = new_marked_object;
  }
}

void
EditorOverlayWidget::select_object()
{
  delete_markers();
  if (!m_dragged_object || !m_dragged_object->is_valid()) return;

  if (m_dragged_object->has_variable_size())
  {
    m_selected_object = m_dragged_object;
    m_dragged_object->editor_select();
    return;
  }

  auto path_obj = dynamic_cast<PathObject*>(m_dragged_object.get());

  if (path_obj && path_obj->get_path_gameobject()) {
    edit_path(path_obj->get_path_gameobject(), m_dragged_object.get());
  }
}

void
EditorOverlayWidget::grab_object()
{
  if (m_hovered_object)
  {
    if (!m_hovered_object->is_valid())
    {
      m_hovered_object = nullptr;
    }
    else
    {
      m_dragged_object = m_hovered_object;
      m_obj_mouse_desync = m_sector_pos - m_hovered_object->get_pos();

      m_dragged_object->save_state();

      auto* pm = dynamic_cast<MarkerObject*>(m_hovered_object.get());
      if (pm)
        m_last_node_marker = dynamic_cast<NodeMarker*>(pm);
      else
        select_object();
    }
  }
  else
  {
    m_dragged_object = nullptr;

    if (m_edited_path &&
        m_editor.get_tileselect_object() == "#node" &&
        m_edited_path->is_valid())
    {
      // do nothing
    }
    else
    {
      delete_markers();
    }
  }
}

void
EditorOverlayWidget::clone_object()
{
  if (m_hovered_object && m_hovered_object->is_saveable())
  {
    if (!m_hovered_object->is_valid())
    {
      m_hovered_object = nullptr;
      return;
    }

    if (dynamic_cast<MarkerObject*>(m_hovered_object.get()))
      return;

    m_obj_mouse_desync = m_sector_pos - m_hovered_object->get_pos();

    auto obj = GameObjectFactory::instance().create(m_hovered_object->get_class_name(), m_hovered_object->save());

    auto* path_object = dynamic_cast<PathObject*>(obj.get());
    if (path_object)
      path_object->editor_clone_path(dynamic_cast<PathObject*>(m_hovered_object.get())->get_path_gameobject());

    m_dragged_object = static_cast<MovingObject*>(&m_editor.get_sector()->add_object(std::move(obj)));
    m_dragged_object->after_editor_set();
  }
  else
  {
    m_dragged_object = nullptr;
    delete_markers();
  }
}

void
EditorOverlayWidget::show_object_menu(GameObject& object)
{
  auto menu = std::make_unique<ObjectMenu>(&object);
  m_editor.m_deactivate_request = true;
  MenuManager::instance().push_menu(std::move(menu));
}

void
EditorOverlayWidget::move_object()
{
  if (m_dragged_object)
  {
    if (!m_dragged_object->is_valid())
    {
      m_dragged_object = nullptr;
      return;
    }
    Vector new_pos = m_sector_pos - m_obj_mouse_desync;
    if (g_config->editor_snap_to_grid)
    {
      auto& snap_grid_size = snap_grid_sizes[g_config->editor_selected_snap_grid_size];
      new_pos = glm::floor(new_pos / static_cast<float>(snap_grid_size)) * static_cast<float>(snap_grid_size);

      auto pm = dynamic_cast<MarkerObject*>(m_dragged_object.get());
      if (pm)
        new_pos -= pm->get_offset();
    }

    // TODO: Temporarily disabled during ongoing discussion
    // Special case: Bezier markers should influence each other when holding shift
    //if (alt_pressed) {
    //  auto bm = dynamic_cast<BezierMarker*>(m_dragged_object);
    //  if (bm) {
    //    auto nm = bm->get_parent();
    //    if (nm) {
    //      nm->move_other_marker(bm->get_uid(), nm->get_pos() * 2.f - new_pos);
    //    } else {
    //      log_warning << "Moving bezier handles without parent NodeMarker" << std::endl;
    //    }
    //  }
    //}

    m_dragged_object->move_to(new_pos);
  }
}

void
EditorOverlayWidget::rubber_object()
{
  if (!m_edited_path) {
    delete_markers();
  }

  if (m_dragged_object) {
    m_dragged_object->editor_delete();
  }

  m_last_node_marker = nullptr;
}

void
EditorOverlayWidget::rubber_rect()
{
  delete_markers();
  Rectf dr = drag_rect();
  for (auto& moving_object : m_editor.get_sector()->get_objects_by_type<MovingObject>())
  {
    const Rectf& bbox = moving_object.get_bbox();
    if (dr.overlaps(bbox)) {
      moving_object.editor_delete();
    }

  }
  m_last_node_marker = nullptr;
}

void
EditorOverlayWidget::update_node_iterators()
{
  if (!m_edited_path) return;
  if (!m_edited_path->is_valid()) return;

  auto* sector = m_editor.get_sector();
  for (auto& marker : sector->get_objects_by_type<NodeMarker>())
  {
    marker.update_iterator();
  }
}

void
EditorOverlayWidget::add_path_node()
{
  m_edited_path->save_state();

  Path::Node new_node(&m_edited_path->get_path());
  new_node.position = m_sector_pos;
  new_node.bezier_before = new_node.position;
  new_node.bezier_after = new_node.position;
  new_node.time = 1;
  m_edited_path->get_path().m_nodes.insert(m_last_node_marker->m_node + 1, new_node);
  auto& bezier_before = Sector::get().add<BezierMarker>(&(*(m_edited_path->get_path().m_nodes.end() - 1)), &((m_edited_path->get_path().m_nodes.end() - 1)->bezier_before));
  auto& bezier_after = Sector::get().add<BezierMarker>(&(*(m_edited_path->get_path().m_nodes.end() - 1)), &((m_edited_path->get_path().m_nodes.end() - 1)->bezier_after));
  auto& new_marker = Sector::get().add<NodeMarker>(m_edited_path->get_path().m_nodes.end() - 1, m_edited_path->get_path().m_nodes.size() - 1, bezier_before.get_uid(), bezier_after.get_uid());
  bezier_before.set_parent(new_marker.get_uid());
  bezier_after.set_parent(new_marker.get_uid());
  //last_node_marker = dynamic_cast<NodeMarker*>(marker.get());
  update_node_iterators();
  new_marker.update_node_times();
  m_editor.get_sector()->flush_game_objects();

  // This will ensure that we will hover NodeMarkers in priority before BezierMarkers
  hover_object();

  grab_object();

  m_edited_path->check_state();
}

void
EditorOverlayWidget::put_object()
{
  const std::string& object_class = m_editor.get_tileselect_object();
  if (object_class[0] == '#')
  {
    if (m_edited_path && object_class == "#node") {
      if (m_edited_path->is_valid() && m_last_node_marker) {
        add_path_node();
      }
    }
  }
  else
  {
    auto target_pos = m_sector_pos;
    if (g_config->editor_snap_to_grid)
    {
      auto& snap_grid_size = snap_grid_sizes[g_config->editor_selected_snap_grid_size];
      target_pos = glm::floor(m_sector_pos / static_cast<float>(snap_grid_size)) * static_cast<float>(snap_grid_size);
    }

    auto object = GameObjectFactory::instance().create(object_class, target_pos);
    object->after_editor_set();

    auto* mo = dynamic_cast<MovingObject*>(object.get());
    if (mo)
    {
      if (!g_config->editor_snap_to_grid)
      {
        auto bbox = mo->get_bbox();
        mo->move_to(mo->get_pos() - Vector(bbox.get_width() / 2, bbox.get_height() / 2));
      }

      auto* wo = dynamic_cast<worldmap::WorldMapObject*>(mo);
      if (wo)
        wo->move_to(wo->get_pos() / 32.0f);
    }

    m_editor.get_sector()->add_object(std::move(object));
  }
}

void
EditorOverlayWidget::process_left_click()
{
  if (MenuManager::instance().has_dialog()) return;
  m_dragging = true;
  m_dragging_right = false;
  m_drag_start = m_sector_pos;

  switch (m_editor.get_tileselect_input_type())
  {
    case EditorTilebox::InputType::TILE:
      switch (m_editor.get_tileselect_select_mode())
      {
        case 0:
          put_tiles(m_hovered_tile, m_editor.get_tiles());
          m_hovered_tile_prev = m_hovered_tile;
          m_time_prev_put_tile = std::chrono::steady_clock::now();
          break;

        case 1:
          preview_rectangle();
          break;

        case 2:
          fill();
          break;

        case 3:
          replace();
          break;

        default:
          break;
      }
      break;

    case EditorTilebox::InputType::NONE:
    case EditorTilebox::InputType::OBJECT:
      switch (m_editor.get_tileselect_move_mode())
      {
        case 0:
          grab_object();
          break;

        case 1:
          clone_object();
          break;

        default:
          break;
      }

      if (!m_editor.get_tileselect_object().empty())
      {
        if (!m_dragged_object) put_object();
      }
      else
      {
        rubber_object();
      }
      break;

    default:
      break;
  }
}

void
EditorOverlayWidget::process_right_click()
{
  switch (m_editor.get_tileselect_input_type())
  {
    case EditorTilebox::InputType::TILE:
      m_dragging = true;
      m_dragging_right = true;
      m_drag_start = m_sector_pos;
      update_tile_selection();
      break;

    case EditorTilebox::InputType::NONE:
    case EditorTilebox::InputType::OBJECT:
      {
        if (m_hovered_object &&
            m_hovered_object->is_valid() &&
            m_hovered_object->has_settings())
        {
          show_object_menu(*m_hovered_object);
        }
      }
      break;

    default:
      break;
  }
}

void
EditorOverlayWidget::process_middle_click()
{
  m_previous_mouse_pos = m_mouse_pos;
  m_scrolling = true;
}

Rectf
EditorOverlayWidget::tile_drag_rect() const
{
  Rectf result = drag_rect();

  // Increase drag rectangle size to the
  // nearest tile border respectively.
  result = Rectf(floorf(result.get_left() / 32) * 32,
                 floorf(result.get_top() / 32) * 32,
                 ceilf(result.get_right() / 32) * 32,
                 ceilf(result.get_bottom() / 32) * 32);
  result.set_p1(sp_to_tp(result.p1()));
  result.set_p2(sp_to_tp(result.p2()));
  return result;
}

Rectf
EditorOverlayWidget::selection_draw_rect() const
{
  Rectf select = tile_drag_rect();
  select.set_p1(tile_screen_pos(select.p1()));
  select.set_p2(tile_screen_pos(select.p2()));
  return select;
}

void
EditorOverlayWidget::update_tile_selection()
{
  Rectf select = tile_drag_rect();
  auto tiles = m_editor.get_tiles();
  auto tileset = m_editor.get_tileset();
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap) return;

  m_selection_warning = false;
  tiles->m_tiles.clear();
  tiles->m_width = static_cast<int>(select.get_width());
  tiles->m_height = static_cast<int>(select.get_height());

  for (int y = static_cast<int>(select.get_top()); y < static_cast<int>(select.get_bottom()); y++)
  {
    for (int x = static_cast<int>(select.get_left()); x < static_cast<int>(select.get_right()); x++)
    {
      if (!is_position_inside_tilemap(tilemap, Vector(x, y)))
      {
        tiles->m_tiles.push_back(0);
      }
      else
      {
        uint32_t tile_id = tilemap->get_tile_id(x, y);
        if (tileset->get(tile_id).is_deprecated())
        {
          set_warning(_("Cannot select deprecated tiles"), 3.f);
          m_selection_warning = true;

          // Clear selections with deprecated tiles
          tiles->clear();
          return;
        }
        tiles->m_tiles.push_back(tile_id);
      }
    }
  }

  update_autotileset();
}

bool
EditorOverlayWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  if (button.button == SDL_BUTTON_LEFT)
  {
    if (m_editor.get_tileselect_input_type() == EditorTilebox::InputType::TILE)
    {
      if (m_dragging && m_editor.get_tileselect_select_mode() == 1)
      {
        put_tiles(sp_to_tp(drag_rect().p1()), m_rectangle_preview.get());
        m_rectangle_preview->m_tiles.clear();
      }

      m_editor.get_selected_tilemap()->check_state();
    }
    else
    {
      if (m_dragging && m_dragged_object) {
        m_dragged_object->check_state();
      }
    }
  }
  else if (button.button == SDL_BUTTON_MIDDLE)
  {
    m_scrolling = false;
  }

  m_dragging = false;

  // Return true anyways, because that's how it worked when this function only
  // had `m_dragging = false;` in its body.
  return true;
}

bool
EditorOverlayWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  switch (button.button)
  {
    case SDL_BUTTON_LEFT:
      process_left_click();
      return true;

    case SDL_BUTTON_RIGHT:
      process_right_click();
      return true;

    case SDL_BUTTON_MIDDLE:
      process_middle_click();
      return true;

    default:
      return false;
  }
}

bool
EditorOverlayWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  update_pos();

  if (m_dragging)
  {
    switch (m_editor.get_tileselect_input_type())
    {
      case EditorTilebox::InputType::TILE:
        if (m_dragging_right)
        {
          update_tile_selection();
        }
        else
        {
          switch (m_editor.get_tileselect_select_mode())
          {
            case 0:
              put_next_tiles();
              break;
            case 1:
              preview_rectangle();
              break;
            default:
              break;
          }
        }
        break;

      case EditorTilebox::InputType::NONE:
      case EditorTilebox::InputType::OBJECT:
        if (m_editor.get_tileselect_object().empty())
        {
          if (m_editor.get_tileselect_select_mode() == 1)
          {
            rubber_rect();
          }
        }
        else
        {
          move_object();
        }
        break;

      default:
        break;
    }
    return true;
  }
  else if (m_scrolling)
  {
    m_editor.scroll(m_previous_mouse_pos - m_mouse_pos);
    m_previous_mouse_pos = m_mouse_pos;
    return true;
  }
  else
  {
    return false;
  }
}

bool
EditorOverlayWidget::on_key_up(const SDL_KeyboardEvent& key)
{
  std::uint16_t mod = key.keysym.mod;

  if (mod & KMOD_SHIFT)
  {
    g_config->editor_snap_to_grid = !g_config->editor_snap_to_grid;
  }
  else if (!m_editor.m_ctrl_pressed)
  {
    m_autotile_mode = g_config->editor_autotile_mode;

    // Hovered objects depend on if ctrl is pressed
    hover_object();
  }
  else if (mod & KMOD_ALT)
  {
    alt_pressed = false;
  }
  return true;
}

bool
EditorOverlayWidget::on_key_down(const SDL_KeyboardEvent& key)
{
  SDL_Keycode sym = key.keysym.sym;
  std::uint16_t mod = key.keysym.mod;

  if (sym == SDLK_F8)
  {
    g_config->editor_render_grid = !g_config->editor_render_grid;
  }
  else if (sym == SDLK_F7 || mod & KMOD_SHIFT)
  {
    g_config->editor_snap_to_grid = !g_config->editor_snap_to_grid;
  }
  else if (sym == SDLK_F5)
  {
    g_config->editor_autotile_mode = !g_config->editor_autotile_mode;
    m_autotile_mode = g_config->editor_autotile_mode;
  }
  else if (m_editor.m_ctrl_pressed)
  {
    m_autotile_mode = !g_config->editor_autotile_mode;
    // Hovered objects depend on if ctrl is pressed.
    hover_object();
  }
  else if (mod & KMOD_ALT)
  {
    alt_pressed = true;
  }
  else if (sym == SDLK_0 || sym == SDLK_KP_0)
  {
    m_current_autotileset = 0;
  }
  else if (sym >= SDLK_1 && sym <= SDLK_9)
  {
    m_current_autotileset = static_cast<int>(sym - SDLK_1 + 1);
  }
  else if (sym >= SDLK_KP_1 && sym <= SDLK_KP_9)
  {
    m_current_autotileset = static_cast<int>(sym - SDLK_KP_1 + 1);
  }
  return true;
}

void
EditorOverlayWidget::on_window_resize()
{
  update_pos();
}

void
EditorOverlayWidget::update_pos()
{
  if(m_editor.get_sector() == nullptr) return;

  m_sector_pos = m_mouse_pos / m_editor.get_sector()->get_camera().get_current_scale() +
                 m_editor.get_sector()->get_camera().get_translation();
  m_hovered_tile = sp_to_tp(m_sector_pos);

  if (m_last_hovered_tile != m_hovered_tile)
  {
    const uint32_t hovered_id = m_editor.get_selected_tilemap()->get_tile_id(m_hovered_tile);
    if ((!m_dragging || hovered_id != 0) &&
        m_editor.get_tiles()->pos(0, 0) == 0 && // Erasing
        m_editor.get_selected_tilemap()->get_tile_id(m_last_hovered_tile) != hovered_id)
    {
      update_autotileset();
    }

    m_last_hovered_tile = m_hovered_tile;
  }

  // update tip
  hover_object();
}

void
EditorOverlayWidget::update_autotileset()
{
  AutotileSet* old_autotileset = get_current_autotileset();

  if (m_editor.get_tiles()->pos(0, 0) == 0) // Erasing
  {
    const uint32_t current_tile = m_editor.get_selected_tilemap()->get_tile_id(m_hovered_tile);
    m_available_autotilesets = m_editor.get_tileset()->get_autotilesets_from_tile(current_tile);
  }
  else
  {
    m_available_autotilesets = m_editor.get_tileset()->get_autotilesets_from_tile(m_editor.get_tiles()->pos(0, 0));
  }

  if (!old_autotileset)
  {
    m_current_autotileset = 0;
    return;
  }
  auto it_autotileset = std::find(m_available_autotilesets.begin(), m_available_autotilesets.end(), old_autotileset);
  m_current_autotileset = it_autotileset != m_available_autotilesets.end() ? static_cast<int>(it_autotileset - m_available_autotilesets.begin()) : 0;
}

AutotileSet*
EditorOverlayWidget::get_current_autotileset() const
{
  if (m_available_autotilesets.empty())
    return nullptr;

  if (m_current_autotileset < 0 || m_current_autotileset > static_cast<int>(m_available_autotilesets.size() - 1))
    return m_available_autotilesets.front();

  return m_available_autotilesets[m_current_autotileset];
}

std::string
EditorOverlayWidget::get_autotileset_key_range() const
{
  if (m_available_autotilesets.size() < 2)
    return "";

  return "(0-" + std::to_string(std::min(static_cast<int>(m_available_autotilesets.size() - 1), 9)) + ")";
}

void
EditorOverlayWidget::draw_tile_tip(DrawingContext& context)
{
  if (m_editor.get_tileselect_input_type() == EditorTilebox::InputType::TILE)
  {
    auto tilemap = m_editor.get_selected_tilemap();
    if (!tilemap) return;

    if (m_editor.get_tiles()->empty()) return;

    const Vector screen_corner = context.get_cliprect().p2();
    Vector drawn_tile(0.f, 0.f);
    auto tiles = m_editor.get_tiles();

    for (drawn_tile.x = static_cast<float>(tiles->m_width) - 1.0f; drawn_tile.x >= 0.0f; drawn_tile.x--)
    {
      for (drawn_tile.y = static_cast<float>(tiles->m_height) - 1.0f; drawn_tile.y >= 0.0f; drawn_tile.y--)
      {
        Vector on_tile = m_hovered_tile + drawn_tile;

        if (!is_position_inside_tilemap(tilemap, on_tile) ||
            on_tile.x >= ceilf(screen_corner.x / 32) ||
            on_tile.y >= ceilf(screen_corner.y / 32))
        {
          continue;
        }
        uint32_t tile_id = tiles->pos(static_cast<int>(drawn_tile.x), static_cast<int>(drawn_tile.y));
        m_editor.get_tileset()->get(tile_id).draw(context.color(),
                                                  align_to_tilemap(on_tile),
                                                  LAYER_GUI - 11, Color(1, 1, 1, 0.5));
        //if (tile_id) {
        //const Tile* tg_tile = m_editor.get_tileset()->get( tile_id );
        //tg_tile->draw(context.color(), tp_to_sp(on_tile),
        //              LAYER_GUI-11, Color(1, 1, 1, 0.5));
        //}
      }
    }
  }
}

void
EditorOverlayWidget::draw_rectangle_preview(DrawingContext& context)
{
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap) return;

  if (m_rectangle_preview->empty()) return;

  Vector screen_corner = context.get_cliprect().p2();
  Vector drawn_tile(0.0f, 0.0f);
  Vector corner(std::min(sp_to_tp(m_drag_start).x, m_hovered_tile.x),
                std::min(sp_to_tp(m_drag_start).y, m_hovered_tile.y));
  auto tiles = m_rectangle_preview.get();

  for (drawn_tile.x = static_cast<float>(tiles->m_width) - 1.0f; drawn_tile.x >= 0.0f; drawn_tile.x--)
  {
    for (drawn_tile.y = static_cast<float>(tiles->m_height) - 1.0f; drawn_tile.y >= 0.0f; drawn_tile.y--)
    {
      Vector on_tile = corner + drawn_tile;

      if (!is_position_inside_tilemap(tilemap, on_tile) ||
          on_tile.x >= ceilf(screen_corner.x / 32) ||
          on_tile.y >= ceilf(screen_corner.y / 32))
      {
        continue;
      }
      uint32_t tile_id = tiles->pos(static_cast<int>(drawn_tile.x), static_cast<int>(drawn_tile.y));
      m_editor.get_tileset()->get(tile_id).draw(context.color(),
                                                align_to_tilemap(on_tile),
                                                LAYER_GUI - 11, Color(1, 1, 1, 0.5));
    }
  }
}

void
EditorOverlayWidget::draw_tile_grid(DrawingContext& context, int tile_size, bool draw_shadow) const
{
  auto current_tm = m_editor.get_selected_tilemap();
  if (current_tm == nullptr) return;

  const Camera& camera = m_editor.get_sector()->get_camera();
  const Rectf draw_rect = Rectf(camera.get_translation(),
                                Sizef((context.get_width() - 128.f) / camera.get_current_scale(),
                                      (context.get_height() - 32.f) / camera.get_current_scale()));
  Vector start = sp_to_tp( Vector(draw_rect.get_left(), draw_rect.get_top()), tile_size );
  Vector end = sp_to_tp( Vector(draw_rect.get_right(), draw_rect.get_bottom()), tile_size );
  start.x = std::max(0.0f, start.x);
  start.y = std::max(0.0f, start.y);
  end.x = std::min(static_cast<float>(current_tm->get_width() * (32 / tile_size)), end.x);
  end.y = std::min(static_cast<float>(current_tm->get_height() * (32 / tile_size)), end.y);

  Vector line_start(0.0f, 0.0f);
  Vector line_end(0.0f, 0.0f);
  auto draw_line = [&](const Vector& from, const Vector& to, const Color& col)
  {
    context.color().draw_line(from, to, col, current_tm->get_layer());
  };
  if (draw_shadow)
  {
    Vector viewport_scale = VideoSystem::current()->get_viewport().get_scale();
    const Color shadow_colour(0.0f, 0.0f, 0.0f, 0.05f);
    const Vector shadow_offset(1.0f / viewport_scale.x,
      1.0f / viewport_scale.y);
    for (int i = static_cast<int>(start.x); i <= static_cast<int>(end.x); i++)
    {
      line_start = tile_screen_pos(Vector(static_cast<float>(i), 0.0f),
        tile_size) + shadow_offset;
      line_end = tile_screen_pos(Vector(static_cast<float>(i), end.y),
        tile_size) + shadow_offset;
      draw_line(line_start, line_end, shadow_colour);
    }

    for (int i = static_cast<int>(start.y); i <= static_cast<int>(end.y); i++)
    {
      line_start = tile_screen_pos(Vector(0.0f, static_cast<float>(i)),
        tile_size) + shadow_offset;
      line_end = tile_screen_pos(Vector(end.x, static_cast<float>(i)),
        tile_size) + shadow_offset;
      draw_line(line_start, line_end, shadow_colour);
    }
  }

  const Color line_color(1.f, 1.f, 1.f, 0.2f);
  for (int i = static_cast<int>(start.x); i <= static_cast<int>(end.x); i++)
  {
    line_start = tile_screen_pos(Vector(static_cast<float>(i), 0.0f), tile_size);
    line_end = tile_screen_pos(Vector(static_cast<float>(i), end.y), tile_size);
    draw_line(line_start, line_end, line_color);
  }

  for (int i = static_cast<int>(start.y); i <= static_cast<int>(end.y); i++)
  {
    line_start = tile_screen_pos(Vector(0.0f, static_cast<float>(i)), tile_size);
    line_end = tile_screen_pos(Vector(end.x, static_cast<float>(i)), tile_size);
    draw_line(line_start, line_end, line_color);
  }
}

void
EditorOverlayWidget::draw_tilemap_border(DrawingContext& context)
{
  if (!m_editor.get_selected_tilemap()) return;

  auto current_tm = m_editor.get_selected_tilemap();
  if (!current_tm) return;

  Vector start = tile_screen_pos( Vector(0, 0) );
  Vector end = tile_screen_pos( Vector(static_cast<float>(current_tm->get_width()),
                                       static_cast<float>(current_tm->get_height())) );
  context.color().draw_line(start, Vector(start.x, end.y), Color(1, 0, 1), current_tm->get_layer());
  context.color().draw_line(start, Vector(end.x, start.y), Color(1, 0, 1), current_tm->get_layer());
  context.color().draw_line(Vector(start.x, end.y), end, Color(1, 0, 1), current_tm->get_layer());
  context.color().draw_line(Vector(end.x, start.y), end, Color(1, 0, 1), current_tm->get_layer());
}

void
EditorOverlayWidget::draw_path(DrawingContext& context)
{
  if (!m_edited_path) return;
  if (!m_selected_object) return;
  if (!m_selected_object->is_valid()) return;
  if (!m_edited_path->is_valid()) return;

  for (auto i = m_edited_path->get_path().m_nodes.begin(); i != m_edited_path->get_path().m_nodes.end(); ++i)
  {
    auto j = i+1;
    Path::Node* node1 = &(*i);
    Path::Node* node2;
    if (j == m_edited_path->get_path().m_nodes.end())
    {
      if (m_edited_path->get_path().m_mode == WalkMode::CIRCULAR)
      {
        //loop to the first node
        node2 = &(*m_edited_path->get_path().m_nodes.begin());
      }
      else
      {
        // Just draw the bezier lines
        context.color().draw_line(node1->position,
                                  node1->bezier_before,
                                  Color(0, 0, 1), LAYER_GUI - 21);
        context.color().draw_line(node1->position,
                                  node1->bezier_after,
                                  Color(0, 0, 1), LAYER_GUI - 21);
        continue;
      }
    }
    else
    {
      node2 = &(*j);
    }
    Bezier::draw_curve(context,
                       node1->position,
                       node1->bezier_after,
                       node2->bezier_before,
                       node2->position,
                       100,
                       Color::RED,
                       LAYER_GUI - 21);
    context.color().draw_line(node1->position,
                              node1->bezier_before,
                              Color(0, 0, 1), LAYER_GUI - 21);
    context.color().draw_line(node1->position,
                              node1->bezier_after,
                              Color(0, 0, 1), LAYER_GUI - 21);
    //context.color().draw_line(node1->position,
    //                          node2->position,
    //                          Color(1, 0, 0), LAYER_GUI - 21);
  }
}

void
EditorOverlayWidget::draw(DrawingContext& context)
{
  if (g_config->editor_render_grid)
  {
    draw_tile_grid(context, 32, true);
    draw_tilemap_border(context);
    auto snap_grid_size = snap_grid_sizes[g_config->editor_selected_snap_grid_size];
    if (snap_grid_size != 32)
    {
      draw_tile_grid(context, snap_grid_size, false);
    }
  }

  m_object_tip->draw(context, m_mouse_pos);

  // Draw zoom indicator.
  // The placing on the top-right is temporary, will be moved with the implementation of an editor toolbar.
  const float scale = m_editor.get_sector()->get_camera().get_current_scale();
  const int scale_percentage = static_cast<int>(roundf(scale * 100.f));
  if (scale_percentage != 100)
    context.color().draw_text(Resources::big_font, std::to_string(scale_percentage) + '%',
                              Vector(context.get_width() - 140.f, 15.f),
                              ALIGN_RIGHT, LAYER_OBJECTS + 1, Color::WHITE);

  context.push_transform();
  context.set_translation(m_editor.get_sector()->get_camera().get_translation());
  context.transform().scale = scale;

  draw_tile_tip(context);
  draw_rectangle_preview(context);
  draw_path(context);

  if (m_editor.get_tileselect_input_type() == EditorTilebox::InputType::TILE &&
      !g_config->editor_show_deprecated_tiles) // If showing deprecated tiles is enabled, this is redundant, since tiles are indicated without the need of hovering over.
  {
    // Deprecated tiles in active tilemaps should have indication, when hovered
    auto sel_tilemap = m_editor.get_selected_tilemap();
    if (m_editor.get_tileset()->get(sel_tilemap->get_tile_id(m_hovered_tile)).is_deprecated())
      context.color().draw_text(Resources::normal_font, "!",
                                tp_to_sp(Vector(static_cast<int>(m_hovered_tile.x), static_cast<int>(m_hovered_tile.y))) + Vector(16, 8),
                                ALIGN_CENTER, LAYER_GUI - 10, Color::RED);
  }

  if (m_dragging && m_editor.get_tileselect_select_mode() == 1
      && !m_dragging_right)
  {
    // Draw selection rectangle...
    Vector p0 = m_drag_start;
    Vector p3 = m_sector_pos;
    if (p0.x > p3.x) {
      std::swap(p0.x, p3.x);
    }

    if (p0.y > p3.y) {
      std::swap(p0.y, p3.y);
    }

    Vector p1 = Vector(p0.x, p3.y);
    Vector p2 = Vector(p3.x, p0.y);

    context.color().draw_filled_rect(Rectf(p0, p1 + Vector(2, 2)),
                                       Color(0.0f, 1.0f, 0.0f, 1.0f), 0.0f, LAYER_GUI-5);
    context.color().draw_filled_rect(Rectf(p2, p3 + Vector(2, 2)),
                                       Color(0.0f, 1.0f, 0.0f, 1.0f), 0.0f, LAYER_GUI-5);
    context.color().draw_filled_rect(Rectf(p0, p2 + Vector(2, 2)),
                                       Color(0.0f, 1.0f, 0.0f, 1.0f), 0.0f, LAYER_GUI-5);
    context.color().draw_filled_rect(Rectf(p1, p3 + Vector(2, 2)),
                                       Color(0.0f, 1.0f, 0.0f, 1.0f), 0.0f, LAYER_GUI-5);

    context.color().draw_filled_rect(Rectf(p0, p3),
                                       Color(0.0f, 1.0f, 0.0f, 0.2f), 0.0f, LAYER_GUI-5);
  }

  context.pop_transform();

  if (m_dragging && m_dragging_right)
  {
    Color selection_color = m_selection_warning ? EditorOverlayWidget::error_color : Color(0.2f, 0.4f, 1.0f);
    selection_color.alpha = 0.6f;
    context.color().draw_filled_rect(selection_draw_rect(), selection_color,
                                     0.0f, LAYER_GUI-13);
  }

  if (m_warning_timer.get_timeleft() > 0.f) // Draw warning, if set
  {
    if (m_warning_text.empty())
      m_warning_timer.stop();
    else
      context.color().draw_text(Resources::normal_font, m_warning_text, Vector(144, 16), ALIGN_LEFT, LAYER_OBJECTS+1, EditorOverlayWidget::warning_color);
  }

  if (g_config->editor_autotile_help)
  {
    if (m_autotile_mode)
    {
      AutotileSet* autotileset = get_current_autotileset();
      if (m_editor.get_tiles()->pos(0, 0) == 0)
      {
        if (autotileset)
        {
          context.color().draw_text(Resources::normal_font, fmt::format(fmt::runtime(_("Autotile erasing mode is on (\"{}\")")), autotileset->get_name()) + " " + get_autotileset_key_range(), Vector(144, 16), ALIGN_LEFT, LAYER_OBJECTS+1, EditorOverlayWidget::text_autotile_active_color);
        }
        else
        {
          context.color().draw_text(Resources::normal_font, _("Autotile erasing cannot be performed here"), Vector(144, 16), ALIGN_LEFT, LAYER_OBJECTS+1, EditorOverlayWidget::text_autotile_error_color);
        }
      }
      else if (autotileset)
      {
        context.color().draw_text(Resources::normal_font, fmt::format(fmt::runtime(_("Autotile mode is on (\"{}\")")), autotileset->get_name()) + " " + get_autotileset_key_range(), Vector(144, 16), ALIGN_LEFT, LAYER_OBJECTS+1, EditorOverlayWidget::text_autotile_active_color);
      }
      else
      {
        context.color().draw_text(Resources::normal_font, _("Selected tile isn't autotileable"), Vector(144, 16), ALIGN_LEFT, LAYER_OBJECTS+1, EditorOverlayWidget::text_autotile_error_color);
      }
    }
    else if (m_editor.get_tiles()->pos(0, 0) == 0)
    {
      context.color().draw_text(Resources::normal_font, _("Hold Ctrl to enable autotile erasing") + " " + get_autotileset_key_range(), Vector(144, 16), ALIGN_LEFT, LAYER_OBJECTS+1, EditorOverlayWidget::text_autotile_available_color);
    }
    else
    {
      context.color().draw_text(Resources::normal_font, _("Hold Ctrl to enable autotile") + " " + get_autotileset_key_range(), Vector(144, 16), ALIGN_LEFT, LAYER_OBJECTS+1, EditorOverlayWidget::text_autotile_available_color);
    }
  }
}

Vector
EditorOverlayWidget::tp_to_sp(const Vector& tp, int tile_size) const
{
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap) return Vector(0, 0);

  Vector sp = tp * static_cast<float>(tile_size);
  return sp + tilemap->get_offset();
}

Vector
EditorOverlayWidget::sp_to_tp(const Vector& sp, int tile_size) const
{
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap) return Vector(0, 0);

  Vector sp_ = sp - tilemap->get_offset();
  return sp_ / static_cast<float>(tile_size);
}

Vector
EditorOverlayWidget::tile_screen_pos(const Vector& tp, int tile_size) const
{
  Vector sp = tp_to_sp(tp, tile_size);
  return (sp - m_editor.get_sector()->get_camera().get_translation()) *
         m_editor.get_sector()->get_camera().get_current_scale();
}

Vector
EditorOverlayWidget::align_to_tilemap(const Vector& sp, int tile_size) const
{
  auto tilemap = m_editor.get_selected_tilemap();
  if (!tilemap) return Vector(0, 0);

  Vector sp_ = sp + tilemap->get_offset() / static_cast<float>(tile_size);
  return glm::trunc(sp_) * static_cast<float>(tile_size);
}

/* EOF */
