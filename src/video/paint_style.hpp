//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_PAINT_STYLE_HPP
#define HEADER_SUPERTUX_VIDEO_PAINT_STYLE_HPP

#include "video/blend.hpp"
#include "video/color.hpp"
#include "video/flip.hpp"

class PaintStyle final
{
public:
  PaintStyle() :
    m_color(Color::WHITE),
    m_alpha(1.0f),
    m_blend(),
    m_flip(NO_FLIP)
  {}

  PaintStyle& set_color(const Color& color) {
    m_color = color;
    return *this;
  }

  PaintStyle& set_alpha(const float& alpha) {
    m_alpha = alpha;
    return *this;
  }

  PaintStyle& set_blend(const Blend& blend) {
    m_blend = blend;
    return *this;
  }

  PaintStyle& set_flip(const Flip& flip) {
    m_flip = flip;
    return *this;
  }

  inline const Color& get_color() const { return m_color; }
  inline const float& get_alpha() const { return m_alpha; }
  inline const Blend& get_blend() const { return m_blend; }
  inline const Flip& get_flip() const { return m_flip; }

private:
  Color m_color;
  float m_alpha;
  Blend m_blend;
  Flip m_flip;
};

#endif

/* EOF */
