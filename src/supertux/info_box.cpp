//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "supertux/info_box.hpp"

#include "supertux/globals.hpp"
#include "supertux/info_box_line.hpp"
#include "util/log.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

InfoBox::InfoBox(const std::string& text) :
  firstline(0),
  // Split text string lines into a vector
  lines(InfoBoxLine::split(text, 400)),
  images(),
  arrow_scrollup(),
  arrow_scrolldown()
{
  try
  {
    // get the arrow sprites
    arrow_scrollup   = Surface::from_file("images/engine/menu/scroll-up.png");
    arrow_scrolldown = Surface::from_file("images/engine/menu/scroll-down.png");
  }
  catch (std::exception& e)
  {
    log_warning << "Could not load scrolling images: " << e.what() << std::endl;
    arrow_scrollup.reset();
    arrow_scrolldown.reset();
  }
}

void
InfoBox::draw(DrawingContext& context)
{
  float x1 = context.get_width() / 2.0f - 200.0f;
  float y1 = context.get_height() / 2.0f - 200.0f;
  float width = 400.0f;
  float height = 200.0f;

  context.color().draw_filled_rect(Rectf(Vector(x1, y1),
                                         Sizef(width, height)),
                                   Color(0.6f, 0.7f, 0.8f, 0.5f), LAYER_GUI - 1);

  float y = y1;
  bool linesLeft = false;
  for (size_t i = firstline; i < lines.size(); ++i) {
    if (y >= y1 + height) {
      linesLeft = true;
      break;
    }

    lines[i]->draw(context, Rectf(x1, y, x1+width, y), LAYER_GUI);
    y += lines[i]->get_height();
  }

  {
    // draw the scrolling arrows
    if (arrow_scrollup.get() && firstline > 0)
      context.color().draw_surface(arrow_scrollup,
                                   Vector(x1 + width  - static_cast<float>(arrow_scrollup->get_width()),  // top-right corner of box
                                          y1), LAYER_GUI);

    if (arrow_scrolldown.get() && linesLeft && firstline < lines.size()-1)
      context.color().draw_surface(arrow_scrolldown,
                                   Vector(x1 + width  - static_cast<float>(arrow_scrolldown->get_width()),  // bottom-light corner of box
                                          y1 + height - static_cast<float>(arrow_scrolldown->get_height())),
                                   LAYER_GUI);
  }
}

void
InfoBox::scrollup()
{
  if (firstline > 0)
    firstline--;
}

void
InfoBox::scrolldown()
{
  if (firstline < lines.size()-1)
    firstline++;
}

void
InfoBox::pageup()
{
}

void
InfoBox::pagedown()
{
}

/* EOF */
