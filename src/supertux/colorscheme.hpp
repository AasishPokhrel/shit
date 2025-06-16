//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_COLORSCHEME_HPP
#define HEADER_SUPERTUX_SUPERTUX_COLORSCHEME_HPP

#include "video/color.hpp"

class ColorScheme final
{
public:
  class Menu
  {
  public:
    static Color back_color;
    static Color front_color;
    static Color help_back_color;
    static Color help_front_color;
    static Color hl_color;
    static Color default_color;
    static Color active_color;
    static Color inactive_color;
    static Color label_color;
    static Color field_color;
    static Color warning_color;
  };

  class Text
  {
  public:
    static Color small_color;
    static Color heading_color;
    static Color reference_color;
    static Color normal_color;
  };

  class Editor
  {
  public:
    static Color default_color;
    static Color hover_color;
    static Color grab_color;
  };
};

#endif

/* EOF */
