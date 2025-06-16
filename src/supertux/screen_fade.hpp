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

#ifndef HEADER_SUPERTUX_SUPERTUX_SCREEN_FADE_HPP
#define HEADER_SUPERTUX_SUPERTUX_SCREEN_FADE_HPP

class DrawingContext;

/**
 * Screen to be displayed simultaneously with another Screen.
 *
 * This is used for Screen transition effects like a fade-out or a shrink-fade
 */
class ScreenFade
{
public:
  enum class FadeType
  {
    NONE,
    FADE,
    CIRCLE
  };

  virtual ~ScreenFade() {}

  /** returns true if the effect is completed */
  virtual bool done() const = 0;

  /** gets called once per frame. The ScreenFade should draw itself in
      this function.  State changes should not be done in this
      function, but rather in update */
  virtual void draw(DrawingContext& context) = 0;

  /** gets called for once (per logical) frame. ScreenFades should do
      their state updates and logic here */
  virtual void update(float dt_sec) = 0;
};

#endif

/* EOF */
