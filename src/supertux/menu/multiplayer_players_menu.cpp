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

#include "supertux/menu/multiplayer_players_menu.hpp"

#include <fmt/format.h>

#include "control/game_controller_manager.hpp"
#include "control/input_manager.hpp"
#include "control/joystick_manager.hpp"
#include "gui/dialog.hpp"
#include "supertux/game_session.hpp"
#include "supertux/menu/multiplayer_player_menu.hpp"
#include "supertux/savegame.hpp"
#include "supertux/sector.hpp"
#include "object/player.hpp"
#include "util/gettext.hpp"

MultiplayerPlayersMenu::MultiplayerPlayersMenu()
{
  add_label(_("Multiplayer"));
  add_hl();

  for (int i = 0; i < InputManager::current()->get_num_users(); i++)
  {
    add_entry(fmt::format(_("Player {}"), i + 1), [i] {
      MenuManager::instance().push_menu(std::make_unique<MultiplayerPlayerMenu>(i));
    });
  }

  add_hl();

  if (InputManager::current()->can_add_user())
  {
    add_entry(_("Add Player"), [] {
      InputManager::current()->push_user();

      if (GameSession::current() && GameSession::current()->get_savegame().get_player_status().m_num_players < InputManager::current()->get_num_users())
      {
        GameSession::current()->get_savegame().get_player_status().add_player();
      }

      MenuManager::instance().set_menu(std::make_unique<MultiplayerPlayersMenu>());
    });
  }

  if (InputManager::current()->get_num_users() > 1)
  {
    add_entry(_("Remove Last Player"), [] {
      if (Sector::current() && Sector::current()->get_object_count<Player>() >= InputManager::current()->get_num_users())
      {
        Dialog::show_confirmation(_("Warning: The player you are trying to\nremove is currently in-game.\n\nDo you wish to remove them anyways?"), [] {

          // Remove the player before the controller, else it'll behave funny
          auto num = InputManager::current()->get_num_users();
          // FIXME: Probably not a good idea to get a player by name
          auto player = Sector::current()->get_object_by_name<Player>("Tux" + std::to_string(num));

          if (player)
          {
            player->remove_me();
          }
          else
          {
            log_warning << "Cannot disconnect player #" << num << " in a sector with " << Sector::current()->get_object_count<Player>() << std::endl;
          }

          InputManager::current()->pop_user();
          MenuManager::instance().set_menu(std::make_unique<MultiplayerPlayersMenu>());
        });
      }
      else
      {
        InputManager::current()->pop_user();
        MenuManager::instance().set_menu(std::make_unique<MultiplayerPlayersMenu>());
      }
    });
  }

  add_hl();
  add_back(_("Back"));
}

/* EOF */
