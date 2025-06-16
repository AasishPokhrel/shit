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

#ifndef HEADER_SUPERTUX_GUI_ITEM_STRINGSELECT_HPP
#define HEADER_SUPERTUX_GUI_ITEM_STRINGSELECT_HPP

#include <functional>

#include "gui/menu_item.hpp"

class ItemStringSelect final : public MenuItem
{
public:
  ItemStringSelect(const std::string& text, std::vector<std::string> items, int* selected, int id = -1);
  ItemStringSelect(const std::string& text, std::vector<std::string> items, int default_item, int id = -1);
  ~ItemStringSelect() override;

  /** Draws the menu item. */
  virtual void draw(DrawingContext&, const Vector& pos, int menu_width, bool active) override;

  /** Returns the minimum width of the menu item. */
  virtual int get_width() const override;

  /** Processes the menu action. */
  virtual void process_action(const MenuAction& action) override;

  virtual bool changes_width() const override {
    return false;
  }

  void set_callback(const std::function<void(int)>& callback) {
    m_callback = callback;
  }

private:
  float calculate_width() const;

private:
  std::vector<std::string> m_items; // list of values for a STRINGSELECT item
  int* m_selected; // currently selected item
  const bool m_pointer_provided; // Indicates whether a pointer has been provided to the item.

  std::function<void(int)> m_callback;
  float m_width;

private:
  ItemStringSelect(const ItemStringSelect&) = delete;
  ItemStringSelect& operator=(const ItemStringSelect&) = delete;
};

#endif

/* EOF */
