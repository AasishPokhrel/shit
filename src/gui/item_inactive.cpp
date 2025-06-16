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

#include "gui/item_inactive.hpp"

#include "supertux/colorscheme.hpp"
#include "video/color.hpp"

ItemInactive::ItemInactive(const std::string& text_, bool default_color) :
  MenuItem(text_),
  m_default_color(default_color)
{
}

Color
ItemInactive::get_color() const {
  return m_default_color ? ColorScheme::Menu::default_color : ColorScheme::Menu::inactive_color;
}

/* EOF */
