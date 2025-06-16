//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_DRAWING_TRANSFORM_HPP
#define HEADER_SUPERTUX_VIDEO_DRAWING_TRANSFORM_HPP

#include "math/vector.hpp"
#include "video/texture.hpp"
#include "video/viewport.hpp"

class DrawingTransform final
{
public:
  Vector translation;
  Rect viewport;
  Flip flip;
  float alpha;
  float scale;

  DrawingTransform(const Viewport& viewport_) :
    translation(0.0f, 0.0f),
    viewport(0, 0,
             viewport_.get_screen_width(), viewport_.get_screen_height()),
    flip(NO_FLIP),
    alpha(1.0f),
    scale(1.0f)
  {}
};

#endif

/* EOF */
