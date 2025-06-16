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

#include "supertux/menu/multiplayer_player_menu.hpp"

#include <fmt/format.h>
#include <SDL.h>

#include "control/game_controller_manager.hpp"
#include "control/input_manager.hpp"
#include "control/joystick_manager.hpp"
#include "gui/dialog.hpp"
#include "object/player.hpp"
#include "supertux/game_session.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/savegame.hpp"
#include "supertux/sector.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

MultiplayerPlayerMenu::MultiplayerPlayerMenu(int player_id)
{
  add_label(fmt::format(_("Player {}"), player_id + 1));
  add_hl();

  add_toggle(-1, _("Play with the keyboard"), &InputManager::current()->m_uses_keyboard[player_id]);

  if (player_id != 0 && GameSession::current()
      && !GameSession::current()->get_savegame().is_title_screen())
  {
    bool player_is_in_sector = false;

    for (const auto* player : GameSession::current()->get_current_sector().get_players())
    {
      if (player->get_id() == player_id)
      {
        player_is_in_sector = true;
        break;
      }
    }

    if (player_is_in_sector)
    {
      add_entry(_("Remove Player"), [player_id] {
        // Re-check everything that concerns the sector, it might have changed
        // (e. g. when unplugging a controller with auto-management enabled)
        if (!GameSession::current() || GameSession::current()->get_savegame().is_title_screen())
        {
          log_warning << "Attempt to force player to despawn while not in session"
                      << std::endl;
          return;
        }

        if (!GameSession::current()->on_player_removed(player_id))
        {
          log_warning << "Could not find player with ID " << player_id
                      << " (number " << (player_id + 1) << "in sector"
                      << std::endl;
        }
      });

      add_entry(_("Respawn Player"), [player_id] {
        // Re-check everything that concerns the sector, it might have changed
        // (e. g. when unplugging a controller with auto-management enabled)
        if (!GameSession::current() || GameSession::current()->get_savegame().is_title_screen())
        {
          log_warning << "Attempt to force player to respawn while not in session"
                      << std::endl;
          return;
        }

        for (auto* player : GameSession::current()->get_current_sector().get_players())
        {
          if (player->get_id() == player_id)
          {
            player->multiplayer_prepare_spawn();
            return;
          }
        }

        log_warning << "Could not find player with ID " << player_id
                    << " (number " << (player_id + 1) << "in sector"
                    << std::endl;
      });
    }
    else
    {
      add_entry(_("Spawn Player"), [player_id] {
        // Re-check everything that concerns the sector, it might have changed
        // (e. g. when unplugging a controller with auto-management enabled)
        if (!GameSession::current() || GameSession::current()->get_savegame().is_title_screen())
        {
          log_warning << "Attempt to force player to spawn while not in session"
                      << std::endl;
          return;
        }

        GameSession::current()->on_player_added(player_id);
      });
    }
  }

  add_hl();
  add_label(_("Controllers"));

  if (InputManager::current()->m_use_game_controller)
  {
    for (auto& pair : InputManager::current()->game_controller_manager->get_controller_mapping())
    {
      auto controller = pair.first;
      std::string prefix = (pair.second == -1) ? "" : (pair.second == player_id) ? "-> " : ("[" + std::to_string(pair.second + 1) + "] ");

      add_entry(prefix + std::string(SDL_GameControllerName(pair.first)), [controller, player_id] {
        InputManager::current()->game_controller_manager->bind_controller(controller, player_id);

        auto err = InputManager::current()->game_controller_manager->rumble(controller);
        switch (err)
        {
          case 1:
            Dialog::show_message(_("This controller does not support rumbling;"
                                   "\nplease check the controllers manually."));
            break;

          case 2:
            Dialog::show_message(_("This SuperTux build does not support "
                                   "rumbling\ncontrollers; please check the "
                                   "controllers manually."));
            break;

          default:
            break;
        }

        MenuManager::instance().set_menu(std::make_unique<MultiplayerPlayerMenu>(player_id));
      });
    }
  }
  else
  {
    for (auto& pair : InputManager::current()->joystick_manager->get_joystick_mapping())
    {
      auto joystick = pair.first;
      std::string prefix = (pair.second == -1) ? "" : (pair.second == player_id) ? "-> " : ("[" + std::to_string(pair.second + 1) + "] ");

      add_entry(prefix + std::string(SDL_JoystickName(pair.first)), [joystick, player_id] {
        InputManager::current()->joystick_manager->bind_joystick(joystick, player_id);

        auto err = InputManager::current()->joystick_manager->rumble(joystick);
        switch (err)
        {
          case 1:
            Dialog::show_message(_("This joystick does not support rumbling;"
                                   "\nplease check the joysticks manually."));
            break;

          case 2:
            Dialog::show_message(_("This SuperTux build does not support "
                                   "rumbling\njoysticks; please check the "
                                   "joysticks manually."));
            break;

          default:
            break;
        }

        MenuManager::instance().set_menu(std::make_unique<MultiplayerPlayerMenu>(player_id));
      });
    }
  }

  add_hl();

  add_back(_("Back"));
}

/* EOF */
