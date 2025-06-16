//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//                2022 Vankata453
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

#ifndef HEADER_SUPERTUX_GUI_ITEM_TEXTFIELD_HPP
#define HEADER_SUPERTUX_GUI_ITEM_TEXTFIELD_HPP

#include "gui/menu_item.hpp"

class ItemTextField : public MenuItem
{
public:
  ItemTextField(const std::string& text_, std::string* input_, int id_ = -1);

  /** Draws the menu item. */
  virtual void draw(DrawingContext&, const Vector& pos, int menu_width, bool active) override;

  /** Returns the minimum width of the menu item. */
  virtual int get_width() const override;

  /** Processes the menu action. */
  virtual void process_action(const MenuAction& action) override;

  std::string* input;

  void change_input(const std::string& input_) {
    *input = input_;
    m_input_undo.clear();
    m_input_redo.clear();
  }

  /** Calls when the user wants to remove an invalid char. */
  virtual void invalid_remove() {}

  /** Processes the given event. */
  virtual void event(const SDL_Event& ev) override;

  /** Indicates that this item changes its width. */
  virtual bool changes_width() const override {
    return true;
  }

  /** Updates undo and redo status. */
  virtual void update_undo();

  /** Calls when the input gets updated. */
  virtual void on_input_update() {}

  // Text manipulation and navigation functions

  virtual void insert_text(const std::string& text, const int left_offset_pos);
  virtual void clear();
  virtual void go_left();
  virtual void go_right();
  virtual void go_to_beginning();
  virtual void go_to_end();
  virtual void delete_front();
  virtual void delete_back();
  virtual void cut();
  virtual void copy();
  virtual void paste();
  virtual void undo();
  virtual void redo();

protected:
  std::string m_input_undo;
  std::string m_input_redo;

  const std::string m_cursor;
  float m_cursor_width;
  int m_cursor_left_offset;

private:
  ItemTextField(const ItemTextField&) = delete;
  ItemTextField& operator=(const ItemTextField&) = delete;
};

#endif

/* EOF */
