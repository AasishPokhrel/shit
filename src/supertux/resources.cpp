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

#include "supertux/resources.hpp"

#include "gui/mousecursor.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/debug.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/tile_manager.hpp"
#include "video/bitmap_font.hpp"
#include "video/font.hpp"
#include "video/surface.hpp"
#include "video/texture_manager.hpp"
#include "video/ttf_font.hpp"
#include "video/ttf_surface_manager.hpp"

void
Resources::reload_all()
{
  TextureManager::current()->reload();

  Resources::load(true);
  SpriteManager::current()->reload();
  TileManager::current()->reload();
}

std::unique_ptr<MouseCursor> Resources::mouse_cursor;

FontPtr Resources::default_font;
FontPtr Resources::console_font;
FontPtr Resources::fixed_font;
FontPtr Resources::normal_font;
FontPtr Resources::small_font;
FontPtr Resources::big_font;
FontPtr Resources::control_font;

SurfacePtr Resources::checkbox;
SurfacePtr Resources::checkbox_checked;
SurfacePtr Resources::back;
SurfacePtr Resources::arrow_left;
SurfacePtr Resources::arrow_right;
SurfacePtr Resources::no_tile;

std::string Resources::current_font;

void
Resources::load(bool reload)
{
  if (!reload)
  {
    // Load the mouse-cursor
    mouse_cursor.reset(new MouseCursor(SpriteManager::current()->create("images/engine/menu/mousecursor.sprite")));
    MouseCursor::set_current(mouse_cursor.get());
  }

  default_font.reset(new TTFFont("fonts/SuperTux-Medium.ttf", 18, 1.25f, 2, 1));
  if (g_debug.get_use_bitmap_fonts())
  {
    console_font.reset(new BitmapFont(BitmapFont::FIXED, "fonts/andale12.stf", 1));
    fixed_font.reset(new BitmapFont(BitmapFont::FIXED, "fonts/white.stf"));
    normal_font.reset(new BitmapFont(BitmapFont::VARIABLE, "fonts/white.stf"));
    small_font.reset(new BitmapFont(BitmapFont::VARIABLE, "fonts/white-small.stf", 1));
    big_font.reset(new BitmapFont(BitmapFont::VARIABLE, "fonts/white-big.stf", 3));
    control_font.reset(new BitmapFont(BitmapFont::FIXED, "fonts/white.stf")); // TODO: Make a better-looking font for this
  }
  else
  {
    console_font.reset(new TTFFont("fonts/SourceCodePro-Medium.ttf", 12, 1.25f, 0, 1));

    auto font = get_font_for_locale(g_dictionary_manager->get_language());
    if(reload || font != current_font)
    {
      current_font = font;
      fixed_font.reset(new TTFFont(font, 18, 1.25f, 2, 1));
      normal_font = fixed_font;
      small_font.reset(new TTFFont(font, 10, 1.25f, 2, 1));
      big_font.reset(new TTFFont(font, 22, 1.25f, 2, 1));
      control_font.reset(new TTFFont("fonts/Roboto-Regular.ttf", 15, 1.25f, 0, 0));
    }
  }
  TTFSurfaceManager::current()->clear_cache();

  /* Load menu images */
  checkbox = Surface::from_file("images/engine/menu/checkbox-unchecked.png");
  checkbox_checked = Surface::from_file("images/engine/menu/checkbox-checked.png");
  back = Surface::from_file("images/engine/menu/arrow-back.png");
  arrow_left = Surface::from_file("images/engine/menu/arrow-left.png");
  arrow_right = Surface::from_file("images/engine/menu/arrow-right.png");
  no_tile = Surface::from_file("images/tiles/auxiliary/notile.png");
}

bool
Resources::needs_custom_font(const tinygettext::Language& locale)
{
  return get_font_for_locale(locale) != "fonts/SuperTux-Medium.ttf";
}

std::string
Resources::get_font_for_locale(const tinygettext::Language& locale)
{
  auto lang = locale.get_language();

  if(lang == "az")
    return "fonts/NotoSans-Regular.ttf";
  if(lang == "ne")
    return "fonts/Dekko-Regular.ttf";
  if(lang == "cmn" || lang == "ja" || lang == "zh")
    return "fonts/NotoSansCJKjp-Medium.otf";
  if(lang == "he")
    return "fonts/VarelaRound-Regular.ttf";
  if(lang == "hy")
    return "fonts/NotoSansArmenian-Regular.ttf";
  if(lang == "km")
    return "fonts/NotoSansKhmer-Regular.ttf";
  if(lang == "ko")
    return "fonts/MapoBackpacking.ttf";
  if(lang == "ml")
    return "fonts/NotoSansMalayalam-Regular.ttf";

  return "fonts/SuperTux-Medium.ttf";
}

void
Resources::unload()
{
  // Free menu images
  no_tile.reset();
  checkbox.reset();
  checkbox_checked.reset();
  back.reset();
  arrow_left.reset();
  arrow_right.reset();

  // Free global images:
  default_font.reset();
  console_font.reset();
  fixed_font.reset();
  normal_font.reset();
  small_font.reset();
  big_font.reset();
  control_font.reset();

  mouse_cursor.reset();
}

Resources::Resources()
{
  load();
}

Resources::~Resources()
{
  unload();
}

/* EOF */
