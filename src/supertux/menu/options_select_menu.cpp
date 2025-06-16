//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#include "supertux/menu/options_select_menu.hpp"

#include "gui/item_horizontalmenu.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/menu/options_menu.hpp"
#include "util/gettext.hpp"

const std::string OptionsSelectMenu::s_icons_base_dir = "images/engine/options/";

OptionsSelectMenu::OptionsSelectMenu(bool complete) :
  m_complete_options(complete)
{
  add_label(_("Options"));
  add_hl();

  ItemHorizontalMenu& horizontal_menu = add_horizontalmenu(MNID_OPTIONCATEGORIES, 150.f, 100.f);
  if (m_complete_options) // Currently, locale options are only available when opened completely.
  {
    horizontal_menu.add_item(_("Locale"), "", s_icons_base_dir + "locale.png", OptionsMenu::LOCALE);
  }
  horizontal_menu.add_item(_("Video"), "", s_icons_base_dir + "video.png", OptionsMenu::VIDEO);
  horizontal_menu.add_item(_("Audio"), "", s_icons_base_dir + "audio.png", OptionsMenu::AUDIO);
  horizontal_menu.add_item(_("Controls"), "", s_icons_base_dir + "controls.png", OptionsMenu::CONTROLS);
  horizontal_menu.add_item(_("Extras"), "", s_icons_base_dir + "extras.png", OptionsMenu::EXTRAS);
  horizontal_menu.add_item(_("Advanced"), "", s_icons_base_dir + "advanced.png", OptionsMenu::ADVANCED);

  add_hl();
  add_back(_("Back"));
}

void
OptionsSelectMenu::menu_action(MenuItem& item)
{
  if (item.get_id() == MNID_OPTIONCATEGORIES)
  {
    const int& type = static_cast<ItemHorizontalMenu&>(item).get_selected_item().id;
    MenuManager::instance().push_menu(std::make_unique<OptionsMenu>(static_cast<OptionsMenu::Type>(type), m_complete_options));
  }
}

/* EOF */
