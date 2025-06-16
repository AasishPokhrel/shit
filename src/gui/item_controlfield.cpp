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

#include "gui/item_controlfield.hpp"

#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"

ItemControlField::ItemControlField(const std::string& text, const std::string& input_, int id) :
  MenuItem(text, id),
  input(input_)
{
}

void
ItemControlField::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active)
{
  context.color().draw_text(Resources::normal_font, input,
                            Vector(pos.x + static_cast<float>(menu_width) - 16.0f,
                                   pos.y - static_cast<float>(int(Resources::normal_font->get_height()/2))),
                            ALIGN_RIGHT, LAYER_GUI, ColorScheme::Menu::field_color);
  context.color().draw_text(Resources::normal_font, get_text(),
                              Vector(pos.x + 16.0f,
                                     pos.y - static_cast<float>(int(Resources::normal_font->get_height()/2))),
                              ALIGN_LEFT, LAYER_GUI, active ? g_config->activetextcolor : get_color());
}

int
ItemControlField::get_width() const {
  return static_cast<int>(Resources::normal_font->get_text_width(get_text()) + Resources::normal_font->get_text_width(input) + 16.0f);
}

/* EOF */
