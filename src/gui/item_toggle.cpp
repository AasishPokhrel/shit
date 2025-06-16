//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "gui/item_toggle.hpp"

#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

ItemToggle::ItemToggle(const std::string& text_, bool* toggled, int id_, bool center_text) :
  MenuItem(text_, id_),
  m_center_text(center_text),
  m_get_func([toggled]{ return *toggled; }),
  m_set_func([toggled](bool value){ *toggled = value; })
{
}

ItemToggle::ItemToggle(const std::string& text_,
                       std::function<bool()> get_func,
                       std::function<void(bool)> set_func,
                       int id_,
                       bool center_text) :
  MenuItem(text_, id_),
  m_center_text(center_text),
  m_get_func(std::move(get_func)),
  m_set_func(std::move(set_func))
{
}

void
ItemToggle::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active)
{
  context.color().draw_text(Resources::normal_font, get_text(),
                            Vector(pos.x + (m_center_text ? static_cast<float>(menu_width) / 2.0f : 16),
                            pos.y - (Resources::normal_font->get_height() / 2)),
                            m_center_text ? ALIGN_CENTER : ALIGN_LEFT, LAYER_GUI, active ? g_config->activetextcolor : get_color());

  if (m_get_func()) {
    context.color().draw_surface(Resources::checkbox_checked,
                                 Vector(pos.x + static_cast<float>(menu_width) - 16.0f - static_cast<float>(Resources::checkbox->get_width()),
                                        pos.y - 8.0f),
                                 LAYER_GUI);
  } else {
    context.color().draw_surface(Resources::checkbox,
                                 Vector(pos.x + static_cast<float>(menu_width) - 16.0f - static_cast<float>(Resources::checkbox->get_width()),
                                        pos.y - 8.0f),
                                 LAYER_GUI);
  }
}

int
ItemToggle::get_width() const
{
  return static_cast<int>(Resources::normal_font->get_text_width(get_text())) + 16 + Resources::checkbox->get_width();
}

void
ItemToggle::process_action(const MenuAction& action)
{
  if (action == MenuAction::HIT) {
    m_set_func(!m_get_func());
  }
}

/* EOF */
