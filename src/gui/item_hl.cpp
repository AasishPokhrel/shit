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

#include "gui/item_hl.hpp"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"

ItemHorizontalLine::ItemHorizontalLine() :
  MenuItem("")
{
}

void
ItemHorizontalLine::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active) {
  // TODO
  /* Draw a horizontal line with a little 3d effect */
  context.color().draw_filled_rect(Rectf(Vector(pos.x, pos.y - 6.0f),
                                         Sizef(static_cast<float>(menu_width), 4.0f)),
                                   Color(g_config->hlcolor), LAYER_GUI);
  context.color().draw_filled_rect(Rectf(Vector(pos.x, pos.y - 6.0f),
                                         Sizef(static_cast<float>(menu_width), 2.0f)),
                                   Color(1.0f, 1.0f, 1.0f, 1.0f), LAYER_GUI);
}

int
ItemHorizontalLine::get_width() const {
  return 0;
}

/* EOF */
