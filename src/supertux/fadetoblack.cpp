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

#include "supertux/fadetoblack.hpp"

#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"

FadeToBlack::FadeToBlack(Direction direction, float fade_time, Color color) :
  m_direction(direction),
  m_fade_time(fade_time),
  m_color(color),
  m_accum_time(0)
{
}

void
FadeToBlack::update(float dt_sec)
{
  m_accum_time += dt_sec;
  if (m_accum_time > m_fade_time)
    m_accum_time = m_fade_time;
}

void
FadeToBlack::draw(DrawingContext& context)
{
  Color col = m_color;
  col.alpha = m_accum_time / m_fade_time;
  if (m_direction != FADEOUT)
    col.alpha = 1.0f - col.alpha;

  // The colours are mixed directly in sRGB space, so change alpha for a more
  // linear fading (it may only work correctly with black).
  col.alpha = Color::remove_gamma(col.alpha);

  context.color().draw_filled_rect(context.get_rect(), col, LAYER_GUI + 1);
}

bool
FadeToBlack::done() const
{
  return m_accum_time >= m_fade_time;
}

/* EOF */
