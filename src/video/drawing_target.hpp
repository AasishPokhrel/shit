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

#ifndef HEADER_SUPERTUX_VIDEO_DRAWING_TARGET_HPP
#define HEADER_SUPERTUX_VIDEO_DRAWING_TARGET_HPP

#include <string>

enum class DrawingTarget {
  /** The color layer, all regular tilemaps and character sprites go
      here. */
  COLORMAP,

  /** The lightmap is drawn on top of the color layer and darkens it,
      elements drawn here act as lightsources. */
  LIGHTMAP
};

DrawingTarget DrawingTarget_from_string(const std::string& text);
std::string to_string(DrawingTarget value);

#endif

/* EOF */
