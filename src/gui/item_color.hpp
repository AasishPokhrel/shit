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

#ifndef HEADER_SUPERTUX_GUI_ITEM_COLOR_HPP
#define HEADER_SUPERTUX_GUI_ITEM_COLOR_HPP

#include "gui/menu_item.hpp"

class Color;

class ItemColor final : public MenuItem
{
public:
  ItemColor(const std::string& text_, Color* color_, int id = -1);

  /** Processes the menu action. */
  virtual void process_action(const MenuAction& action) override;

  virtual Color get_color() const override;

  //int target_menu;

private:
  Color* color;

private:
  ItemColor(const ItemColor&) = delete;
  ItemColor& operator=(const ItemColor&) = delete;
};

#endif

/* EOF */
