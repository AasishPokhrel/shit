//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
//                     Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_FONT_HPP
#define HEADER_SUPERTUX_VIDEO_FONT_HPP

#include <string>

#include "math/rectf.hpp"
#include "math/vector.hpp"
#include "video/color.hpp"
#include "video/surface_ptr.hpp"
#include "video/texture.hpp"

class Canvas;

enum FontAlignment {
  ALIGN_LEFT,
  ALIGN_CENTER,
  ALIGN_RIGHT
};

class Font
{
public:
  /**
   * returns the given string, truncated (preferably at whitespace) to be at most max_chars characters long
   */
  static std::string wrap_to_chars(const std::string& text, int max_chars, std::string* overflow);

public:
  virtual ~Font() {}

  virtual float get_height() const = 0;

  virtual float get_text_width(const std::string& text) const = 0;
  virtual float get_text_height(const std::string& text) const = 0;

  virtual std::string wrap_to_width(const std::string& text, float width, std::string* overflow) = 0;

  virtual Rectf draw_text(Canvas& canvas, const std::string& text,
                          const Vector& pos, FontAlignment alignment, int layer, const Color& color) = 0;
};

#endif

/* EOF */
