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

#ifndef HEADER_SUPERTUX_VIDEO_VIEWPORT_HPP
#define HEADER_SUPERTUX_VIDEO_VIEWPORT_HPP

#include "math/rect.hpp"
#include "math/vector.hpp"

class Viewport final
{
private:
public:
  static Viewport from_size(const Size& target_size, const Size& desktop_size);

public:
  Viewport();
  Viewport(const Rect& rect, const Vector& scale);

  /** The size of the viewport in window coordinates */
  inline Rect get_rect() const { return m_rect; }

  /** The amount by which the content of the viewport is scaled */
  inline Vector get_scale() const { return m_scale; }

  /** The width of the resulting logical screen */
  int get_screen_width() const;

  /** The height of the resulting logical screen */
  int get_screen_height() const;

  /** The size of the resulting logical screen */
  Size get_screen_size() const;

  /** Converts window coordinates into logical screen coordinates */
  Vector to_logical(int physical_x, int physical_y) const;

  /** True if the logical screen doens't cover the whole window */
  bool needs_clear_screen() const;

private:
  /** The minimum logical screen size that is allowed */
  static const Size s_max_size;

  /** The maximum logical screen size that is allowed */
  static const Size s_min_size;

private:
  Rect m_rect;
  Vector m_scale;
};

#endif

/* EOF */
