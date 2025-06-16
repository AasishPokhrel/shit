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

#ifndef HEADER_SUPERTUX_GUI_ITEM_ACTION_HPP
#define HEADER_SUPERTUX_GUI_ITEM_ACTION_HPP

#include "gui/menu_item.hpp"

class ItemAction final : public MenuItem
{
public:
  ItemAction(const std::string& text, int id = -1, std::function<void()> callback = {}, const Color& text_color = Color(1.f, 1.f, 1.f));

  virtual void process_action(const MenuAction& action) override;

private:
  std::function<void()> m_callback;

private:
  ItemAction(const ItemAction&) = delete;
  ItemAction& operator=(const ItemAction&) = delete;
};

#endif

/* EOF */
