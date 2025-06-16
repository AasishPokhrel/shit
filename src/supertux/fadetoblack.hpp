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

#ifndef HEADER_SUPERTUX_SUPERTUX_FADETOBLACK_HPP
#define HEADER_SUPERTUX_SUPERTUX_FADETOBLACK_HPP

#include "supertux/screen_fade.hpp"
#include "video/color.hpp"

/**
 * Fades a screen towards a specific color
 */
class FadeToBlack final : public ScreenFade
{
public:
  enum Direction { FADEOUT, FADEIN };

public:
  FadeToBlack(Direction direction, float fade_time, Color dest_color = Color(0, 0, 0));

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  /// returns true if the effect is completed
  virtual bool done() const override;

private:
  Direction m_direction;
  float m_fade_time;
  Color m_color;
  float m_accum_time;

private:
  FadeToBlack(const FadeToBlack&) = delete;
  FadeToBlack& operator=(const FadeToBlack&) = delete;
};

#endif

/* EOF */
