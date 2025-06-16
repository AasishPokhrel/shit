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

#include "supertux/shrinkfade.hpp"

#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

ShrinkFade::ShrinkFade(const Vector& dest, float fade_time, int draw_layer, Direction direction) :
  m_draw_layer(draw_layer),
  m_dest(dest),
  m_fade_time(fade_time),
  m_accum_time(0),
  m_initial_size(static_cast<float>(SCREEN_HEIGHT > SCREEN_WIDTH ? SCREEN_HEIGHT : SCREEN_WIDTH)),
  m_direction(direction)
{
}

void
ShrinkFade::update(float dt_sec)
{
  m_accum_time += dt_sec;
  if (m_accum_time > m_fade_time)
    m_accum_time = m_fade_time;
}

void
ShrinkFade::draw(DrawingContext& context)
{
  float progress = m_accum_time / m_fade_time;
  float diameter = 2 * m_initial_size * (m_direction == FADEOUT ? (1.0f - progress) : progress);
  context.color().draw_inverse_ellipse(m_dest, Vector(1.1f * diameter, diameter),
                                         Color(0, 0, 0), m_draw_layer);
}

bool
ShrinkFade::done() const
{
  return m_accum_time >= m_fade_time;
}

/* EOF */
