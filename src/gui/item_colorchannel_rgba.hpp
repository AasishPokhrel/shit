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

#ifndef HEADER_SUPERTUX_GUI_ITEM_COLORCHANNEL_RGBA_HPP
#define HEADER_SUPERTUX_GUI_ITEM_COLORCHANNEL_RGBA_HPP

#include "gui/menu_item.hpp"

#include "util/colorspace_oklab.hpp"
#include "video/color.hpp"


class ItemColorChannelRGBA final : public MenuItem
{
public:
  ItemColorChannelRGBA(float* input_, Color channel_, int id_ = -1,
    bool is_linear = false);

  /** Draws the menu item. */
  virtual void draw(DrawingContext&, const Vector& pos, int menu_width, bool active) override;

  /** Returns the minimum width of the menu item. */
  virtual int get_width() const override;

  /** Processes the menu action. */
  virtual void process_action(const MenuAction& action) override;

  /** Processes the given event. */
  virtual void event(const SDL_Event& ev) override;

  virtual Color get_color() const override;

  virtual bool changes_width() const override { return true; }

  void change_input(const std::string& input_) { set_text(input_); }

private:
  void enable_edit_mode();
  void add_char(char c);
  void remove_char();

private:
  float* m_number;
  float m_number_prev;
  bool m_is_linear;
  bool m_edit_mode;
  int m_flickw;
  Color m_channel;

private:
  ItemColorChannelRGBA(const ItemColorChannelRGBA&) = delete;
  ItemColorChannelRGBA& operator=(const ItemColorChannelRGBA&) = delete;
};

#endif

/* EOF */
