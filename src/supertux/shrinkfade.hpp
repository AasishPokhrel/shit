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

#ifndef HEADER_SUPERTUX_SUPERTUX_SHRINKFADE_HPP
#define HEADER_SUPERTUX_SUPERTUX_SHRINKFADE_HPP

#include "math/vector.hpp"
#include "supertux/screen_fade.hpp"
#include "video/layer.hpp"

/** Shrinks a rectangle screen towards a specific position */
class ShrinkFade final : public ScreenFade
{
public:
  enum Direction { FADEOUT, FADEIN };

public:
  ShrinkFade(const Vector& point, float fade_time, int draw_layer = LAYER_GUI + 1, Direction = FADEOUT);

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual bool done() const override;

private:
  const int m_draw_layer;
  Vector m_dest;
  float m_fade_time;
  float m_accum_time;
  float m_initial_size;
  Direction m_direction;

private:
  ShrinkFade(const ShrinkFade&) = delete;
  ShrinkFade& operator=(const ShrinkFade&) = delete;
};

#endif

/* EOF */
