//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

#include "supertux/menu/cheat_apply_menu.hpp"

#include <fmt/format.h>

#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "object/player.hpp"
#include "supertux/game_session.hpp"
#include "supertux/sector.hpp"

CheatApplyMenu::CheatApplyMenu(std::function<void(Player&)> callback) :
  m_callback(callback)
{
  add_label(_("Apply cheat to player"));
  add_hl();

  add_entry(-1, _("All Players"));
  for (const auto player : Sector::get().get_players())
  {
    add_entry(player->get_id(), fmt::format(_("Player {}"), player->get_id() + 1));
  }

  add_hl();
  add_back(_("Back"));
}

void
CheatApplyMenu::menu_action(MenuItem& item)
{
  int id = item.get_id();

  if (id < -1)
    return;

  for (const auto& player : Sector::get().get_players())
  {
    if ((id == -1 || id == player->get_id()) && m_callback)
    {
      m_callback(*player);
    }
  }

  MenuManager::instance().clear_menu_stack();
}

/* EOF */
