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

#include "gui/item_back.hpp"

#include "gui/menu_manager.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

ItemBack::ItemBack(const std::string& text, int id) :
  MenuItem(text, id)
{
}

void
ItemBack::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active)
{
  float text_width = Resources::normal_font->get_text_width(get_text());
  context.color().draw_text(Resources::normal_font, get_text(),
                            Vector( pos.x + static_cast<float>(menu_width) / 2.0f,
                                    pos.y - static_cast<float>(int(Resources::normal_font->get_height()/2))),
                            ALIGN_CENTER, LAYER_GUI, active ? g_config->activetextcolor : get_color());
  context.color().draw_surface(Resources::back,
                               Vector(pos.x + static_cast<float>(menu_width) / 2.0f + text_width / 2.0f  + 16.0f,
                                      pos.y - 8.0f),
                                 LAYER_GUI);
}

int
ItemBack::get_width() const {
  return static_cast<int>(Resources::normal_font->get_text_width(get_text())) + 32 + Resources::back->get_width();
}

void
ItemBack::process_action(const MenuAction& action) {
  if (action == MenuAction::HIT) {
    if (MenuManager::instance().current_menu()->on_back_action())
      MenuManager::instance().pop_menu();
  }
}
/* EOF */
