//  SuperTux
//  Copyright (C) 2020 Grzegorz Przybylski <zwatotem@gmail.com>
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

#include "object/vertical_stripes.hpp"

#include "editor/editor.hpp"
#include "math/rect.hpp"
#include "math/rectf.hpp"
#include "supertux/sector.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

VerticalStripes::VerticalStripes() :
  m_visible(false),
  m_layer(LAYER_FOREGROUND1 + 10),
  m_left_stripe(0,0,0,0),
  m_right_stripe(0,0,0,0)
{
}

VerticalStripes::~VerticalStripes()
{
}

void
VerticalStripes::update(float dt_sec)
{
  m_visible = !Editor::is_active();
}

void
VerticalStripes::draw(DrawingContext& context)
{
  float screen_width = context.get_width();
  float level_width = Sector::get().get_width();
  if (m_visible && level_width < screen_width)
  {
    // Drawing two black stripes at each side of a screen
    float screen_height = context.get_height();
    float level_height = Sector::get().get_height();

    Canvas& canvas = context.get_canvas(DrawingTarget::COLORMAP);

    float screen_left = (level_width - screen_width) / 2;
    float screen_right = level_width - screen_left;
    // Level can still be scrolled vertically!
    float rect_top = -screen_height;
    float rect_bottom = level_height + screen_height;
    Rectf left_stripe = Rectf(screen_left, rect_top, 0, rect_bottom);
    Rectf right_stripe = Rectf(level_width, rect_top, screen_right, rect_bottom);
    canvas.draw_filled_rect(left_stripe, Color(0,0,0), m_layer);
    canvas.draw_filled_rect(right_stripe, Color(0,0,0), m_layer);
  }
}

/* EOF */
