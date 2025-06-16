//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#include "gui/menu_script.hpp"

#include "gui/item_script_line.hpp"
#include "util/gettext.hpp"

ScriptMenu::ScriptMenu(std::string* script_) :
  base_script(script_),
  script_strings()
{
  script_strings.clear();

  add_label(_("Edit script"));
  add_hl();

  // Split the script to the lines.
  std::string script = *base_script;
  std::string line_break = "\n";
  size_t endl_pos = script.find(line_break);
  while (endl_pos != std::string::npos) {
    std::string new_line = script.substr(0, endl_pos);
    script = script.substr(endl_pos + line_break.length());
    push_string(new_line);
    endl_pos = script.find(line_break);
  }
  push_string(script);

  //add_script_line(base_script);

  add_hl();
  add_back(_("OK"));
}

ScriptMenu::~ScriptMenu()
{
  *base_script = *(script_strings[0]);
  for (auto i = script_strings.begin()+1; i != script_strings.end(); ++i) {
    *base_script += "\n" + **i;
  }
}

void
ScriptMenu::push_string(const std::string& new_line)
{
  script_strings.push_back(std::make_unique<std::string>(new_line));
  add_item(std::make_unique<ItemScriptLine>(*this, (script_strings.end()-1)->get()));
}

void
ScriptMenu::remove_line() {
  // The script should have at least one line.
  if (script_strings.size() <= 1) {
    return;
  }

  script_strings.erase(script_strings.begin() + (m_active_item - 2));
  delete_item(m_active_item);
  calculate_height();
}

ItemScriptLine*
ScriptMenu::add_line()
{
  auto new_line = std::make_unique<std::string>();
  script_strings.insert(script_strings.begin() + (m_active_item - 1), std::move(new_line));

  auto line_item = std::make_unique<ItemScriptLine>(*this, (script_strings.begin()+(m_active_item-1))->get());
  add_item(std::move(line_item), m_active_item+1);
  m_active_item++;

  return static_cast<ItemScriptLine*>(m_items[m_active_item].get());
}

void
ScriptMenu::menu_action(MenuItem& item)
{
}

bool
ScriptMenu::is_sensitive() const {
  return true;
}

/* EOF */
