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

#include "editor/toolbox_widget.hpp"

#include "editor/editor.hpp"
#include "editor/tilebox.hpp"
#include "editor/tool_icon.hpp"
#include "gui/menu_manager.hpp"
#include "gui/mousecursor.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/resources.hpp"
#include "util/gettext.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

using InputType = EditorTilebox::InputType;

EditorToolboxWidget::EditorToolboxWidget(Editor& editor) :
  m_editor(editor),
  m_tilebox(new EditorTilebox(editor, Rectf())),
  m_pos_x(0.f),
  m_hovered_item(HoveredItem::NONE),
  m_hovered_tool(-1),
  m_rubber(new ToolIcon("images/engine/editor/rubber.png")),
  m_select_mode(new ToolIcon("images/engine/editor/select-mode0.png")),
  m_node_marker_mode(new ToolIcon("images/engine/editor/path_node.png")),
  m_move_mode(new ToolIcon("images/engine/editor/move-mode0.png")),
  m_undo_mode(new ToolIcon("images/engine/editor/arrow.png")),
  m_has_mouse_focus(false)
{
  m_select_mode->push_mode("images/engine/editor/select-mode1.png");
  m_select_mode->push_mode("images/engine/editor/select-mode2.png");
  m_select_mode->push_mode("images/engine/editor/select-mode3.png");
  m_move_mode->push_mode("images/engine/editor/move-mode1.png");
  m_undo_mode->push_mode("images/engine/editor/redo.png");
  //settings_mode->push_mode("images/engine/editor/settings-mode1.png");
}

void
EditorToolboxWidget::draw(DrawingContext& context)
{
  m_tilebox->draw(context);

  context.color().draw_filled_rect(Rectf(Vector(m_pos_x, 0.f),
                                         Vector(context.get_width(), 96.f)),
                                   g_config->editorcolor,
                                   0.0f, LAYER_GUI-10);

  if (m_hovered_item != HoveredItem::NONE && m_hovered_item != HoveredItem::TILEBOX)
  {
    context.color().draw_filled_rect(get_hovered_item_rect(),
                                     g_config->editorhovercolor,
                                     0.0f, LAYER_GUI - 5);
  }

  context.color().draw_text(Resources::normal_font, _("Tiles"),
                            Vector(context.get_width(), 5),
                            ALIGN_RIGHT, LAYER_GUI, ColorScheme::Menu::default_color);
  context.color().draw_text(Resources::normal_font, _("Objects"),
                            Vector(context.get_width(), 37),
                            ALIGN_RIGHT, LAYER_GUI, ColorScheme::Menu::default_color);

  m_rubber->draw(context);
  m_undo_mode->draw(context);
  switch (m_tilebox->get_input_type())
  {
    case InputType::TILE:
      m_select_mode->draw(context);
      break;

    case InputType::NONE:
    case InputType::OBJECT:
      m_node_marker_mode->draw(context);
      m_move_mode->draw(context);
      break;

    default:
      break;
  }
}

bool
EditorToolboxWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  return m_tilebox->on_mouse_button_up(button);
}

bool
EditorToolboxWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (m_tilebox->on_mouse_button_down(button))
  {
    m_editor.update_autotileset();
    update_mouse_icon();
    return true;
  }

  if (button.button == SDL_BUTTON_LEFT)
  {
    switch (m_hovered_item)
    {
      case HoveredItem::TILEGROUP:
        if (m_editor.get_tileset()->get_tilegroups().size() > 1)
        {
          m_editor.disable_keyboard();
          MenuManager::instance().push_menu(MenuStorage::EDITOR_TILEGROUP_MENU);
        }
        else
        {
          select_tilegroup(0);
        }
        return true;

      case HoveredItem::OBJECTS:
        if ((m_editor.get_level()->is_worldmap() && m_tilebox->get_object_info().get_num_worldmap_groups() > 1) ||
            (!m_editor.get_level()->is_worldmap() && m_tilebox->get_object_info().get_num_level_groups() > 1))
        {
          m_editor.disable_keyboard();
          MenuManager::instance().push_menu(MenuStorage::EDITOR_OBJECTGROUP_MENU);
        }
        else
        {
          if (m_editor.get_level()->is_worldmap())
            select_objectgroup(m_tilebox->get_object_info().get_first_worldmap_group_index());
          else
            select_objectgroup(0);
        }
        return true;

      case HoveredItem::TOOL:
        switch (m_hovered_tool)
        {
          case 0:
            m_tilebox->get_tiles()->set_tile(0);
            m_tilebox->set_object("");
            m_editor.update_autotileset();
            update_mouse_icon();
            break;

          case 1:
            switch (m_tilebox->get_input_type())
            {
              case InputType::TILE:
                m_select_mode->next_mode();
                break;
              case InputType::NONE:
              case InputType::OBJECT:
                m_tilebox->set_object("#node");
                break;
              default:
                break;
            }
            update_mouse_icon();
            break;
 
           case 2:
             if (m_tilebox->get_input_type() == InputType::OBJECT ||
                 m_tilebox->get_input_type() == InputType::NONE)
               m_move_mode->next_mode();
             update_mouse_icon();
             break;
 
           case 3:
             m_tilebox->set_object("#move");
             update_mouse_icon();
             break;

          default:
            break;
        }
        return true;

      default:
        return false;
    }
  }
  else
  {
    return false;
  }
}

bool
EditorToolboxWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  if (m_tilebox->on_mouse_motion(motion))
  {
    m_hovered_item = HoveredItem::TILEBOX;
    m_has_mouse_focus = true;
    return false; // The layers widget wouldn't be able to scroll otherwise.
  }

  const Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  const float x = mouse_pos.x - m_pos_x;
  const float y = mouse_pos.y - m_tilebox->get_rect().get_top();

  if (x < 0)
  {
    m_hovered_item = HoveredItem::NONE;
    m_has_mouse_focus = false;
    return false;
  }

  // Mouse is currently over the toolbox widget.
  m_has_mouse_focus = true;

  if (y < 0) /** Tool bar */
  {
    if (y < -64)
    {
      m_hovered_item = HoveredItem::TILEGROUP;
    }
    else if (y < -32)
    {
      m_hovered_item = HoveredItem::OBJECTS;
    }
    else
    {
      m_hovered_item = HoveredItem::TOOL;
      m_hovered_tool = get_tool_pos(mouse_pos);
    }
    return true;
  }
  else /** Tilebox */
  {
    m_hovered_item = HoveredItem::TILEBOX;
  }

  return false;
}

bool
EditorToolboxWidget::on_mouse_wheel(const SDL_MouseWheelEvent& wheel)
{
  return m_tilebox->on_mouse_wheel(wheel);
}

void
EditorToolboxWidget::on_window_resize()
{
  m_pos_x = static_cast<float>(SCREEN_WIDTH - 128);
  m_tilebox->set_rect(Rectf(Vector(m_pos_x, 96.f),
                            Vector(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT))));

  m_rubber->m_pos           = Vector(m_pos_x        , 64.0f);
  m_select_mode->m_pos      = Vector(m_pos_x + 32.0f, 64.0f);
  m_node_marker_mode->m_pos = Vector(m_pos_x + 32.0f, 64.0f);
  m_move_mode->m_pos        = Vector(m_pos_x + 64.0f, 64.0f);
  m_undo_mode->m_pos        = Vector(m_pos_x + 96.0f, 64.0f);

  m_tilebox->on_window_resize();
}

void
EditorToolboxWidget::setup()
{
  m_tilebox->setup();

  on_window_resize();
  m_tilebox->get_tiles()->set_tile(0);
}

void
EditorToolboxWidget::select_tilegroup(int id)
{
  m_tilebox->select_tilegroup(id);
  update_mouse_icon();
}

void
EditorToolboxWidget::select_objectgroup(int id)
{
  m_tilebox->select_objectgroup(id);
  update_mouse_icon();
}

int
EditorToolboxWidget::get_tileselect_select_mode() const
{
  return m_select_mode->get_mode();
}

int
EditorToolboxWidget::get_tileselect_move_mode() const
{
  return m_move_mode->get_mode();
}

Vector
EditorToolboxWidget::get_tool_coords(int pos) const
{
  return Vector(static_cast<float>((pos % 4) * 32) + m_pos_x,
                static_cast<float>((pos / 4) * 32) + 64.f);
}

int
EditorToolboxWidget::get_tool_pos(const Vector& coords) const
{
  int x = static_cast<int>((coords.x - m_pos_x) / 32.f);
  int y = static_cast<int>((coords.y - 64.f) / 32.f);
  return y * 4 + x;
}

Rectf
EditorToolboxWidget::get_hovered_item_rect() const
{
  switch (m_hovered_item)
  {
    case HoveredItem::TILEGROUP:
      return Rectf(Vector(m_pos_x, 0.f), Vector(static_cast<float>(SCREEN_WIDTH), 32.f));

    case HoveredItem::OBJECTS:
      return Rectf(Vector(m_pos_x, 32.f), Vector(static_cast<float>(SCREEN_WIDTH), 64.f));

    case HoveredItem::TOOL:
    {
      auto coords = get_tool_coords(m_hovered_tool);
      return Rectf(coords, coords + Vector(32.f, 32.f));
    }

    default:
      return Rectf();
  }
}

void
EditorToolboxWidget::update_mouse_icon()
{
  MouseCursor::current()->set_icon(get_mouse_icon()->get_current_surface());
}

ToolIcon*
EditorToolboxWidget::get_mouse_icon() const
{
  switch (m_tilebox->get_input_type())
  {
    case InputType::NONE:
    case InputType::OBJECT:
    {
      const std::string object = m_tilebox->get_object();

      if (object.empty())
        return m_rubber.get();
      if (object == "#node")
        return m_node_marker_mode.get();

      return m_move_mode.get();
    }

    case InputType::TILE:
      return m_select_mode.get();

    default:
      return nullptr;
  }
}

/* EOF */
