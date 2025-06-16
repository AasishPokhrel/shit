//  SuperTux
//  Copyright (C) 2022-2023 Vankata453
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

#ifndef HEADER_SUPERTUX_GUI_ITEM_HORIZONTALMENU_HPP
#define HEADER_SUPERTUX_GUI_ITEM_HORIZONTALMENU_HPP

#include "gui/menu_item.hpp"

#include <vector>

#include "math/rectf.hpp"
#include "video/surface_ptr.hpp"

class DrawingContext;

class ItemHorizontalMenu final : public MenuItem
{
protected:
  static float s_max_width;

  static const float s_width_offset;
  static const float s_menu_width_offset;
  static const float s_item_spacing;

  static const float s_icon_y;

public:
  struct Item
  {
    const int id;
    const std::string text;
    const std::string description;
    const SurfacePtr icon;
  };

protected:
  std::vector<Item> m_items;
  int m_selected_item;
  int m_item_range_begin;
  int m_item_range_end;

  float m_width;
  const float m_height;
  const float m_min_item_width;
  Rectf m_rect;

public:
  ItemHorizontalMenu(int id, float height, float min_item_width = -1.f);

  Item& get_selected_item();

  void add_item(const std::string& text, const std::string& description,
                const std::string& icon_file, int id = -1);

  void draw(DrawingContext& context, const Vector& pos, int menu_width, bool active) override;
  void draw_item(DrawingContext& context, const Item& item, bool active,
                 const Vector& pos, const float& item_width);

  void process_action(const MenuAction& action) override;
  void event(const SDL_Event& ev) override;

  void on_window_resize() override;

  bool changes_width() const override { return true; }
  bool select_blink() const override { return false; }

  float get_distance() const override { return 10.f; }

  int get_width() const override { return static_cast<int>(m_width); }
  int get_height() const override { return static_cast<int>(m_height); }

protected:
  void calculate_width(int begin = 0);
  void calculate_rect(const Vector& pos);

  // Navigation functions
  void go_left();
  void go_right();

private:
  float get_item_width(const std::string& text) const;

private:
  ItemHorizontalMenu(const ItemHorizontalMenu&) = delete;
  ItemHorizontalMenu& operator=(const ItemHorizontalMenu&) = delete;
};

#endif

/* EOF */
