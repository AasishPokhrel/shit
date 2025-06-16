//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//                2024 bruhmoent
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

#include <sstream>

#include "gui/menu_color.hpp"
#include "gui/menu_item.hpp"
#include "util/log.hpp"

#include "util/gettext.hpp"

ColorMenu::ColorMenu(Color* color) :
  m_color(color)
{
  add_label(_("Mix the colour"));
  add_hl();

  add_color_picker_2d(*m_color);
  add_color_channel_rgba(&(m_color->red), Color::RED);
  add_color_channel_rgba(&(m_color->green), Color::GREEN);
  add_color_channel_rgba(&(m_color->blue), Color::BLUE);
  add_color_channel_rgba(&(m_color->alpha), Color::BLACK, -1, true);
  add_color_display(m_color);

  add_hl();
  add_entry(MNID_COPY_CLIPBOARD_RGB, _("Copy to clipboard (rgb)"));
  add_entry(MNID_COPY_CLIPBOARD_HEX, _("Copy to clipboard (hex)"));
  if (SDL_HasClipboardText())
  {
    char* clipboard_text = SDL_GetClipboardText();
    std::optional<Color> clipboard_color;

    if (clipboard_text)
    {
      const std::string text(clipboard_text);
      SDL_free(clipboard_text);

      clipboard_color = Color::deserialize_from_rgb(text);
      if (!clipboard_color)
        clipboard_color = Color::deserialize_from_hex(text);
    }

    add_entry(MNID_PASTE_CLIPBOARD, _("Paste from clipboard"), clipboard_color.value_or(Color(1.f, 1.f, 1.f)));
  }
  else
    add_entry(MNID_PASTE_CLIPBOARD, _("Paste from clipboard"), Color(1.f, 1.f, 1.f));

  add_hl();
  add_back(_("OK"));
}

void
ColorMenu::copy_to_clipboard(const std::string& color_str)
{
  if (SDL_SetClipboardText(color_str.c_str()) != 0)
    log_warning << "Failed to set SDL clipboard text: " << SDL_GetError() << std::endl;

  MenuItem& menu_paste_item = get_item_by_id(MNID_PASTE_CLIPBOARD);
  menu_paste_item.set_text_color(*m_color);
}

void
ColorMenu::menu_action(MenuItem& item)
{
  if (item.get_id() == MNID_COPY_CLIPBOARD_RGB)
  {
    if (m_color)
    {
      const std::string clipboard_text(Color::serialize_to_rgb(*m_color));
      copy_to_clipboard(clipboard_text);
    }
  }
  else if (item.get_id() == MNID_COPY_CLIPBOARD_HEX)
  {
    if (m_color)
    {
      const std::string clipboard_text(Color::serialize_to_hex(*m_color));
      copy_to_clipboard(clipboard_text);
    }
  }
  else if (item.get_id() == MNID_PASTE_CLIPBOARD)
  {
    if (SDL_HasClipboardText())
    {
      char* clipboard_text = SDL_GetClipboardText();
      if (!clipboard_text)
        return;

      const std::string text(clipboard_text);
      SDL_free(clipboard_text);

      std::optional<Color> clipboard_color = Color::deserialize_from_rgb(text);
      if (!clipboard_color)
        clipboard_color = Color::deserialize_from_hex(text);

      if (clipboard_color)
      {
        *m_color = *clipboard_color;
        MenuItem& menu_paste_item = get_item_by_id(MNID_PASTE_CLIPBOARD);
        menu_paste_item.set_text_color(*clipboard_color);
      }
      else
        log_warning << "Invalid color format: " << text << ". Supported formats: rgb(r,g,b) and #rrggbb" << std::endl;
    }
  }
}

/* EOF */
