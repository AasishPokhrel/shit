//  SuperTux -- List menu
//  Copyright (C) 2021 Rami <rami.slicer@gmail.com>
//                2023 Vankata453
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
#include "gui/menu_list.hpp"
#include "gui/menu_manager.hpp"
#include "util/gettext.hpp"

ListMenu::ListMenu(const std::vector<std::string>& entries, std::string* selected, Menu* parent,
                   const std::function<std::string (const std::string&)>& text_processor) :
  m_entries(entries),
  m_selected(selected),
  m_parent(parent)
{
  for (int i = 0; i < static_cast<int>(m_entries.size()); i++)
  {
    if (text_processor)
      add_entry(i, text_processor(m_entries[i]));
    else
      add_entry(i, m_entries[i]);
  }

  add_hl();
  add_back(_("Cancel"));
}

void
ListMenu::menu_action(MenuItem& item)
{
  if (m_selected)
    *m_selected = m_entries.at(item.get_id());

  if (m_parent)
    m_parent->refresh();

  MenuManager::instance().pop_menu();
}

/* EOF */
