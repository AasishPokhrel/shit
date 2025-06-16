//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2015 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_GUI_MENU_ITEM_HPP
#define HEADER_SUPERTUX_GUI_MENU_ITEM_HPP

#include "gui/menu.hpp"
#include <optional>

class MenuItem
{
public:
  explicit MenuItem(const std::string& text, int id = -1, const std::optional<Color>& text_color = std::nullopt);
  virtual ~MenuItem();

  inline int get_id() const { return m_id; }

  void set_help(const std::string& help_text);
  inline const std::string& get_help() const { return m_help; }

  inline void set_text(const std::string& text) { m_text = text; }
  inline const std::string& get_text() const { return m_text; }

  inline void set_font(const FontPtr font) { m_font = font; }
  inline const FontPtr& get_font() const { return m_font; }

  /** Draws the menu item. */
  virtual void draw(DrawingContext&, const Vector& pos, int menu_width, bool active);

  virtual void on_window_resize() {}

  /** Returns true when the menu item has no action and therefore can be skipped.
      Useful for labels and horizontal lines.*/
  virtual bool skippable() const {
    return false;
  }

  /** Returns the distance between the items above and below the current
      menu item. */
  virtual float get_distance() const { return 0.f; }

  /** Returns the minimum width of the menu item. */
  virtual int get_width() const;

  /** Returns height of menu item. */
  virtual int get_height() const { return 24; }

  /** Processes the menu action. */
  virtual void process_action(const MenuAction& action) { }

  /** Processes the given event. */
  virtual void event(const SDL_Event& ev) { }

  virtual Color get_color() const;
  virtual void set_text_color(const Color& color);

  /** Returns true when the MenuManager shouldn't do anything else. */
  virtual bool no_other_action() const {
    return false;
  }

  /** Returns true when the width must be recalculated when an action is
      processed */
  virtual bool changes_width() const {
    return false;
  }

  /** Returns true when the item should have a blink effect, provided by the menu,
      when active. */
  virtual bool select_blink() const { return true; }

private:
  int m_id;
  std::string m_text;
  std::string m_help;
  FontPtr m_font;
  std::optional<Color> m_text_color;

private:
  MenuItem(const MenuItem&) = delete;
  MenuItem& operator=(const MenuItem&) = delete;
};

#endif

/* EOF */
