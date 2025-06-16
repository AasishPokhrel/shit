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

#ifndef HEADER_SUPERTUX_EDITOR_OVERLAY_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_OVERLAY_WIDGET_HPP

#include <SDL.h>
#include <chrono>

#include "control/input_manager.hpp"
#include "editor/tile_selection.hpp"
#include "editor/widget.hpp"
#include "math/vector.hpp"
#include "object/tilemap.hpp"
#include "supertux/timer.hpp"
#include "util/typed_uid.hpp"

class AutotileSet;
class Color;
class DrawingContext;
class Editor;
class GameObject;
class MovingObject;
class NodeMarker;
class Path;
class Rectf;
class Tip;

/** A widget that is drawn on top of the current sector and handles
    mouse input and tool drawing. */
class EditorOverlayWidget final : public Widget
{
public:
  static Color text_autotile_available_color;
  static Color text_autotile_active_color;
  static Color text_autotile_error_color;
  static Color warning_color;
  static Color error_color;

public:
  EditorOverlayWidget(Editor& editor);
  ~EditorOverlayWidget() override;

  virtual void draw(DrawingContext&) override;
  virtual void update(float dt_sec) override;

  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;
  virtual bool on_key_up(const SDL_KeyboardEvent& key) override;
  virtual bool on_key_down(const SDL_KeyboardEvent& key) override;
  virtual void on_window_resize() override;

  void update_pos();
  void update_autotileset();
  void delete_markers();
  void update_node_iterators();
  void on_level_change();

  void edit_path(PathGameObject* path, GameObject* new_marked_object = nullptr);
  //void reset_action_press();

private:
  static bool action_pressed;
  static bool alt_pressed;

private:
  void input_tile(const Vector& pos, uint32_t tile);
  void input_autotile(const Vector& pos, uint32_t tile);
  void input_autotile_erase(const Vector& pos);
  void put_tiles(const Vector& target_tile, TileSelection* tiles);
  void put_next_tiles();
  void draw_rectangle();
  void preview_rectangle();
  bool check_tiles_for_fill(uint32_t replace_tile, uint32_t target_tile, uint32_t third_tile) const;
  void fill();
  void replace();
  void put_object();

  void rubber_object();
  void rubber_rect();

  void grab_object();
  void move_object();
  void clone_object();
  void hover_object();
  void show_object_menu(GameObject& object);
  void select_object();
  void add_path_node();

  AutotileSet* get_current_autotileset() const;
  std::string get_autotileset_key_range() const;

  void draw_tile_tip(DrawingContext&);
  void draw_tile_grid(DrawingContext&, int tile_size, bool draw_shadow) const;
  void draw_tilemap_border(DrawingContext&);
  void draw_path(DrawingContext&);
  void draw_rectangle_preview(DrawingContext& context);

  void process_left_click();
  void process_right_click();
  void process_middle_click();

  // sp is sector pos, tp is pos on tilemap.
  Vector tp_to_sp(const Vector& tp, int tile_size = 32) const;
  Vector sp_to_tp(const Vector& sp, int tile_size = 32) const;
  Vector tile_screen_pos(const Vector& tp, int tile_size = 32) const;
  Vector align_to_tilemap(const Vector& sp, int tile_size = 32) const;

  // in sector position
  Rectf drag_rect() const;
  Rectf tile_drag_rect() const;
  Rectf selection_draw_rect() const;
  void update_tile_selection();

  inline void set_warning(const std::string& text, float time)
  {
    m_warning_text = text;
    m_warning_timer.start(time);
  }

private:
  Editor& m_editor;
  Vector m_hovered_tile;
  Vector m_hovered_tile_prev;
  Vector m_last_hovered_tile;
  Vector m_sector_pos;
  Vector m_mouse_pos;
  Vector m_previous_mouse_pos;

  std::chrono::steady_clock::time_point m_time_prev_put_tile;

  bool m_dragging;
  bool m_dragging_right;
  bool m_scrolling;
  Vector m_drag_start;
  TypedUID<MovingObject> m_dragged_object;

  TypedUID<MovingObject> m_hovered_object;
  TypedUID<GameObject> m_selected_object;
  TypedUID<PathGameObject> m_edited_path;
  TypedUID<NodeMarker> m_last_node_marker;

  std::vector<AutotileSet*> m_available_autotilesets;
  int m_current_autotileset;

  std::unique_ptr<Tip> m_object_tip;
  Vector m_obj_mouse_desync;

  std::unique_ptr<TileSelection> m_rectangle_preview;

  // Warnings
  Timer m_warning_timer;
  std::string m_warning_text;

  bool m_selection_warning;

  /// Not to be confused with g_config.editor_autotile_mode,
  /// this variable is the one that *also* changes when holding CTRL.
  /// The other one only changes hitting F5
  bool m_autotile_mode;

private:
  EditorOverlayWidget(const EditorOverlayWidget&) = delete;
  EditorOverlayWidget& operator=(const EditorOverlayWidget&) = delete;
};

#endif

/* EOF */
