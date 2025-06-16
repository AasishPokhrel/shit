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

#include "gui/item_color.hpp"

#include "gui/menu.hpp"
#include "gui/menu_color.hpp"
#include "gui/menu_manager.hpp"
#include "video/color.hpp"

ItemColor::ItemColor(const std::string& text, Color* color_, int id) :
  MenuItem(text, id),
  color(color_)
{
}

void
ItemColor::process_action(const MenuAction& action) {
  if (action == MenuAction::HIT) {
    MenuManager::instance().push_menu(std::make_unique<ColorMenu>(color));
  }
}

Color
ItemColor::get_color() const {
  return *color;
}

/* EOF */
