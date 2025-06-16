//  SuperTux
//  Copyright (C) 2021 mrkubax10 <mrkubax10@onet.pl>
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

#include "gui/menu_string_array.hpp"

#include <fmt/format.h>

#include "gui/menu_item.hpp"
#include "util/gettext.hpp"

StringArrayMenu::StringArrayMenu(std::vector<std::string>& items) :
  m_array_items(items),
  m_text(),
  m_selected_item(-1)
{
  reload();
}

void
StringArrayMenu::menu_action(MenuItem& item)
{
  int id = item.get_id();
  if (id >= 0)
  {
    m_text = m_array_items[id];
    m_selected_item = id;
    get_item_by_id(-2).set_text(fmt::format(_("Selected item: {}"), (m_selected_item >= 0 ? m_array_items[m_selected_item] : _("None"))));
  }
  else if (m_text.length() > 0 && id < -2)
  {
    if (id == -3)
    {
      m_array_items.push_back(m_text);
    }
    else if (id == -4 && m_selected_item >= 0)
    {
      m_array_items.insert(m_array_items.begin() + m_selected_item + 1, m_text);
    }
    else if (id == -5 && m_selected_item >= 0)
    {
      m_array_items[m_selected_item] = m_text;
    }
    else if (id == -6 && m_selected_item >= 0)
    {
      m_array_items.erase(m_array_items.begin() + m_selected_item);
    }
    m_text = "";
    m_selected_item = -1;
    reload();
  }
}

void
StringArrayMenu::reload()
{
  clear();
  add_label(_("Edit string array"));
  add_hl();
  for (unsigned int i = 0; i < m_array_items.size(); i++)
  {
    add_entry(i, m_array_items.at(i));
  }
  add_hl();
  add_textfield(_("Text"), &m_text);
  add_entry(-2, fmt::format(_("Selected item: {}"), (m_selected_item >= 0 ? m_array_items[m_selected_item] : _("None"))));
  add_entry(-3, _("Add"));
  add_entry(-4, _("Insert"));
  add_entry(-5, _("Update"));
  add_entry(-6, _("Delete"));
  add_hl();
  add_back(_("OK"));
}
/* EOF */
