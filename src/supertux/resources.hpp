//  SuperTux
//  Copyright (C) 2003 Tobias Glaesser <tobi.web@gmx.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_RESOURCES_HPP
#define HEADER_SUPERTUX_SUPERTUX_RESOURCES_HPP

#include <memory>
#include <string>

#include "util/gettext.hpp"
#include "video/font_ptr.hpp"
#include "video/surface_ptr.hpp"

class MouseCursor;

class Resources final
{
public:
  static void reload_all();

public:
  static std::unique_ptr<MouseCursor> mouse_cursor;

  /** Font that can display English glyphs */
  static FontPtr default_font;

  /** Font used in the console */
  static FontPtr console_font;

  /** A version of the regular font with fixed spacing for displaying
      coin counts and other numbers in the HUD */
  static FontPtr fixed_font;

  /** Regular sized font for menus and text scrolls. */
  static FontPtr normal_font;

  /** Small font for statistics, FPS, etc. */
  static FontPtr small_font;

  /** Big font for menu titles and headers in text scrolls */
  static FontPtr big_font;

  /** Font used for control interface elements (particle editor) */
  static FontPtr control_font;

  static SurfacePtr checkbox;
  static SurfacePtr checkbox_checked;
  static SurfacePtr back;
  static SurfacePtr arrow_left;
  static SurfacePtr arrow_right;
  static SurfacePtr no_tile;

public:
  static void load(bool reload = false);
  static void unload();
  static bool needs_custom_font(const tinygettext::Language& locale);
  static std::string get_font_for_locale(const tinygettext::Language& locale);

private:
  static std::string current_font;

public:
  Resources();
  ~Resources();
};

#endif

/* EOF */
