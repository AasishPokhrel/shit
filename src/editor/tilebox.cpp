//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//                2024 Vankata453
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

#include "editor/tilebox.hpp"

#include "editor/editor.hpp"
#include "editor/object_info.hpp"
#include "editor/tile_selection.hpp"
#include "editor/tip.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/debug.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/resources.hpp"
#include "util/log.hpp"
#include "video/drawing_context.hpp"

EditorTilebox::EditorTilebox(Editor& editor, const Rectf& rect) :
  m_editor(editor),
  m_rect(rect),
  m_tiles(new TileSelection()),
  m_object(),
  m_object_tip(new Tip()),
  m_input_type(InputType::NONE),
  m_active_tilegroup(),
  m_active_objectgroup(),
  m_object_info(new ObjectInfo()),
  m_on_select_callback([](EditorTilebox&) {}),
  m_scrollbar(),
  m_scroll_progress(1.f),
  m_hovered_item(HoveredItem::NONE),
  m_hovered_tile(-1),
  m_dragging(false),
  m_drag_start(0, 0),
  m_mouse_pos(0, 0)
{
  m_scrollbar.reset(new ControlScrollbar(1.f, 1.f, m_scroll_progress, 35.f));
}

void
EditorTilebox::draw(DrawingContext& context)
{
  context.color().draw_filled_rect(m_rect,
                                   g_config->editorcolor,
                                   0.0f, LAYER_GUI-10);

  if (m_dragging)
  {
    context.color().draw_filled_rect(selection_draw_rect(), Color(0.2f, 0.4f, 1.0f, 0.6f),
                                     0.0f, LAYER_GUI+1);
  }

  if (m_hovered_item == HoveredItem::TILE)
  {
    m_object_tip->draw(context, Vector(m_mouse_pos.x + 25, m_mouse_pos.y), true);

    context.color().draw_filled_rect(get_tile_rect(),
                                     g_config->editorhovercolor,
                                     0.0f, LAYER_GUI - 5);
  }

  context.push_transform();
  context.set_viewport(Rect(m_rect));
  switch (m_input_type)
  {
    case InputType::TILE:
      draw_tilegroup(context);
      break;

    case InputType::OBJECT:
      draw_objectgroup(context);
      break;

    default:
      break;
  }
  context.pop_transform();

  if (m_hovered_item != HoveredItem::NONE)
    m_scrollbar->draw(context);
}

void
EditorTilebox::draw_tilegroup(DrawingContext& context)
{
  int pos = -1;
  for (auto& tile_ID : m_active_tilegroup->tiles)
  {
    pos++;
    if (pos / 4 < static_cast<int>(m_scroll_progress / 32.f))
      continue;

    auto position = get_tile_coords(pos, false);
    m_editor.get_tileset()->get(tile_ID).draw(context.color(), position, LAYER_GUI - 9);

    if (g_config->developer_mode && (m_active_tilegroup->developers_group || g_debug.show_toolbox_tile_ids) && tile_ID != 0)
    {
      // Display tile ID on top of tile:
      context.color().draw_text(Resources::console_font, std::to_string(tile_ID),
                                position + Vector(16, 16), ALIGN_CENTER, LAYER_GUI - 9, Color::WHITE);
    }
  }
}

void
EditorTilebox::draw_objectgroup(DrawingContext& context)
{
  int pos = -1;
  for (auto& icon : m_active_objectgroup->get_icons())
  {
    pos++;
    if (pos / 4 < static_cast<int>(m_scroll_progress / 32.f))
      continue;

    icon.draw(context, get_tile_coords(pos, false));
  }
}

Rectf
EditorTilebox::normalize_selection(bool rounded) const
{
  const float tile_scroll_progress = (rounded ? floorf(m_scroll_progress / 32.f) : m_scroll_progress / 32.f);

  Vector drag_start = m_drag_start - Vector(0.f, tile_scroll_progress);
  Vector drag_end = Vector(static_cast<float>(m_hovered_tile % 4),
                           static_cast<float>(m_hovered_tile / 4) - tile_scroll_progress); // NOLINT
  if (drag_start.x > drag_end.x) {
    std::swap(drag_start.x, drag_end.x);
  }
  if (drag_start.y > drag_end.y) {
    std::swap(drag_start.y, drag_end.y);
  }
  return Rectf(drag_start, drag_end);
}

Rectf
EditorTilebox::selection_draw_rect() const
{
  Rectf select = normalize_selection(false);
  select.set_p2(select.p2() + Vector(1, 1));
  select.set_p1((select.p1() * 32.0f) + m_rect.p1());
  select.set_p2((select.p2() * 32.0f) + m_rect.p1());

  if (select.get_top() < m_rect.get_top()) // Do not go outside toolbox
    select.set_top(m_rect.get_top());

  return select;
}

void
EditorTilebox::update_selection()
{
  Rectf select = normalize_selection(true);
  m_tiles->m_tiles.clear();
  m_tiles->m_width = static_cast<int>(select.get_width() + 1);
  m_tiles->m_height = static_cast<int>(select.get_height() + 1);

  int size = static_cast<int>(m_active_tilegroup->tiles.size());
  for (int y = static_cast<int>(select.get_top()); y <= static_cast<int>(select.get_bottom()); y++)
  {
    for (int x = static_cast<int>(select.get_left()); x <= static_cast<int>(select.get_right()); x++)
    {
      int tile_pos = (y + static_cast<int>(m_scroll_progress / 32.f)) * 4 + x;
      if (tile_pos < size && tile_pos >= 0)
        m_tiles->m_tiles.push_back(m_active_tilegroup->tiles[tile_pos]);
      else
        m_tiles->m_tiles.push_back(0);
    }
  }
}

bool
EditorTilebox::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  m_scrollbar->on_mouse_button_up(button);

  if (m_dragging)
  {
    m_dragging = false;
    m_on_select_callback(*this);
  }
  return false;
}

bool
EditorTilebox::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (m_scrollbar->on_mouse_button_down(button))
    return true;

  if (button.button == SDL_BUTTON_LEFT)
  {
    if (m_hovered_item == HoveredItem::TILE)
    {
      switch (m_input_type)
      {
        case InputType::TILE:
          {
            m_dragging = true;
            m_drag_start = Vector(static_cast<float>(m_hovered_tile % 4),
                                  static_cast<float>(m_hovered_tile / 4)); // NOLINT

            int size = static_cast<int>(m_active_tilegroup->tiles.size());
            if (m_hovered_tile < size && m_hovered_tile >= 0)
              m_tiles->set_tile(m_active_tilegroup->tiles[m_hovered_tile]);
            else
              m_tiles->set_tile(0);
          }
          break;

        case InputType::OBJECT:
          {
            int size = static_cast<int>(m_active_objectgroup->get_icons().size());
            if (m_hovered_tile < size && m_hovered_tile >= 0)
            {
              m_object = m_active_objectgroup->get_icons()[m_hovered_tile].get_object_class();
              m_on_select_callback(*this);
            }
          }
          break;

        default:
          break;
      }
      return true;
    }
  }
  return false;
}

bool
EditorTilebox::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  if (m_scrollbar->on_mouse_motion(motion))
  {
    m_hovered_item = HoveredItem::SCROLLBAR;
    m_object_tip->set_visible(false);
    return true;
  }

  m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);

  if (!m_rect.contains(m_mouse_pos))
  {
    m_hovered_item = HoveredItem::NONE;
    m_object_tip->set_visible(false);
    return false;
  }

  update_hovered_tile();
  return true;
}

bool
EditorTilebox::on_mouse_wheel(const SDL_MouseWheelEvent& wheel)
{
  if (m_hovered_item == HoveredItem::TILE)
  {
    m_scrollbar->on_mouse_wheel(wheel);
    update_hovered_tile();
  }

  return false;
}

void
EditorTilebox::update_hovered_tile()
{
  const int prev_hovered_tile = std::move(m_hovered_tile);
  m_hovered_item = HoveredItem::TILE;
  m_hovered_tile = get_tile_pos(m_mouse_pos);
  if (m_dragging && m_input_type == InputType::TILE)
  {
    update_selection();
  }
  else if (m_input_type == InputType::OBJECT && m_hovered_tile != prev_hovered_tile)
  {
    const auto& icons = m_active_objectgroup->get_icons();
    if (m_hovered_tile < static_cast<int>(icons.size())) {
      const std::string obj_class = icons[m_hovered_tile].get_object_class();
      std::string obj_name = obj_class;
      try {
        obj_name = GameObjectFactory::instance().get_display_name(obj_class);
      }
      catch (const std::exception& err) {
        log_warning << "Unable to get display name of object '" << obj_class << "': " << err.what() << std::endl;
      }
      m_object_tip->set_info(obj_name);
    }
    else {
      m_object_tip->set_visible(false);
    }
  }
}

void
EditorTilebox::on_window_resize()
{
  m_scrollbar->set_covered_region(m_rect.get_height());
  m_scrollbar->set_total_region(get_tiles_height());
  m_scrollbar->set_rect(Rectf(Vector(m_rect.get_right() - 10.f, m_rect.get_top()), m_rect.p2()));
}

void
EditorTilebox::setup()
{
  on_window_resize();
  m_tiles->set_tile(0);
}

void
EditorTilebox::set_rect(const Rectf& rect)
{
  m_rect = rect;
  on_window_resize();

  m_hovered_item = HoveredItem::NONE;
  m_hovered_tile = -1;
  m_dragging = false;
  m_drag_start = Vector(0.f, 0.f);
}

Vector
EditorTilebox::get_tile_coords(int pos, bool relative) const
{
  return Vector(static_cast<float>((pos % 4) * 32) + (relative ? m_rect.get_left() : 0),
                static_cast<float>((pos / 4) * 32) + (relative ? m_rect.get_top() : 0) - m_scroll_progress);
}

int
EditorTilebox::get_tile_pos(const Vector& coords) const
{
  int x = static_cast<int>((coords.x - m_rect.get_left()) / 32.0f);
  int y = static_cast<int>((coords.y - m_rect.get_top() + m_scroll_progress) / 32.0f);
  return y * 4 + x;
}

Rectf
EditorTilebox::get_tile_rect() const
{
  auto coords = get_tile_coords(m_hovered_tile);
  Rectf rect(coords, coords + Vector(32.f, 32.f));

  if (rect.get_top() < m_rect.get_top()) // Do not go outside toolbox
    rect.set_top(m_rect.get_top());

  return rect;
}

void
EditorTilebox::on_select(const std::function<void(EditorTilebox&)>& callback)
{
  m_on_select_callback = callback;
}

void
EditorTilebox::select_tilegroup(int id)
{
  m_active_tilegroup.reset(new Tilegroup(m_editor.get_tileset()->get_tilegroups()[id]));
  m_input_type = InputType::TILE;
  reset_scrollbar();
}

void
EditorTilebox::select_objectgroup(int id)
{
  m_active_objectgroup = &m_object_info->m_groups[id];
  m_input_type = InputType::OBJECT;
  reset_scrollbar();
}

bool
EditorTilebox::select_layers_objectgroup()
{
  ObjectGroup* layers = m_editor.get_level()->is_worldmap() ?
    m_object_info->m_worldmap_layers_group.get() :
    m_object_info->m_layers_group.get();

  if (layers)
  {
    m_active_objectgroup = layers;
    m_input_type = InputType::OBJECT;
  }
  else
  {
    m_active_objectgroup = nullptr;
    m_input_type = InputType::NONE;
  }
  reset_scrollbar();
  return layers;
}

float
EditorTilebox::get_tiles_height() const
{
  switch (m_input_type)
  {
    case InputType::TILE:
      return ceilf(static_cast<float>(m_active_tilegroup->tiles.size()) / 4.f) * 32.f;

    case InputType::OBJECT:
      return ceilf(static_cast<float>(m_active_objectgroup->get_icons().size()) / 4.f) * 32.f;

    default:
      return 0.f;
  }
}

void
EditorTilebox::reset_scrollbar()
{
  m_scroll_progress = 0.f;
  m_scrollbar->set_total_region(get_tiles_height());
}

/* EOF */
