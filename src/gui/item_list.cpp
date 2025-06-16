//  SuperTux
//  Copyright (C) 2022 mrkubax10 <mrkubax10@onet.pl>
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

#include "gui/item_list.hpp"

#include "gui/menu_manager.hpp"
#include "gui/menu_list.hpp"

ItemList::ItemList(const std::string& text, const std::vector<std::string>& items, std::string* value_ptr, int id) :
  MenuItem(text, id),
  m_items(items),
  m_value_ptr(value_ptr)
{}

void
ItemList::process_action(const MenuAction& action)
{
  if (action == MenuAction::HIT)
    MenuManager::instance().push_menu(std::make_unique<ListMenu>(m_items, m_value_ptr, nullptr));
}

/* EOF */
