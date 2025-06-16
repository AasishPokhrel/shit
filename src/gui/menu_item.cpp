//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2015 Hume2 <teratux.mail@gmail.com>
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

#include "gui/menu_item.hpp"

#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"

static const float HELP_TEXT_WIDTH = 800.f;

MenuItem::MenuItem(const std::string& text, int id, const std::optional<Color>& text_color) :
  m_id(id),
  m_text(text),
  m_help(),
  m_font(Resources::normal_font),
  m_text_color(text_color)
{
}

MenuItem::~MenuItem()
{
}

void
MenuItem::set_help(const std::string& help_text)
{
  std::string overflow;
  m_help = m_font->wrap_to_width(help_text, HELP_TEXT_WIDTH, &overflow);
  while (!overflow.empty())
  {
    m_help += "\n";
    m_help += m_font->wrap_to_width(overflow, HELP_TEXT_WIDTH, &overflow);
  }
}

void
MenuItem::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active)
{
  context.color().draw_text(m_font, m_text,
                            Vector( pos.x + static_cast<float>(menu_width) / 2.0f,
                                    pos.y - static_cast<float>(m_font->get_height()) / 2.0f ),
                            ALIGN_CENTER, LAYER_GUI, active ? g_config->activetextcolor : get_color());
}

Color
MenuItem::get_color() const
{
  return m_text_color.value_or(ColorScheme::Menu::default_color);
}

void
MenuItem::set_text_color(const Color& color)
{
  m_text_color = color;
}

int
MenuItem::get_width() const
{
  return static_cast<int>(m_font->get_text_width(m_text)) + 16;
}

/* EOF */
