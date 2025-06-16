//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/menu/game_menu.hpp"

#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/game_session.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/sector.hpp"
#include "object/player.hpp"
#include "util/gettext.hpp"

GameMenu::GameMenu() :
  reset_callback ( [] {
    MenuManager::instance().clear_menu_stack();
    GameSession::current()->toggle_pause();
    GameSession::current()->reset_button = true;
  }),
  reset_checkpoint_callback( [] {
    MenuManager::instance().clear_menu_stack();
    GameSession::current()->toggle_pause();

    GameSession::current()->reset_checkpoint_button = true;
  }),
  abort_callback([] {
    GameSession::current()->abort_level();
  })
{
  Level& level = GameSession::current()->get_current_level();

  add_label(level.m_name);
  add_hl();
  add_entry(MNID_CONTINUE, _("Continue"));
  add_entry(MNID_RESETLEVEL, _("Restart Level"));

  if (Sector::current()->get_players()[0]->get_status().can_reach_checkpoint()) {
    add_entry(MNID_RESETLEVELCHECKPOINT, _("Restart from Checkpoint"));
  }

  add_submenu(_("Options"), MenuStorage::INGAME_OPTIONS_MENU);
  add_hl();
  add_entry(MNID_ABORTLEVEL, _("Abort Level"));
}

void
GameMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_CONTINUE:
      MenuManager::instance().clear_menu_stack();
      GameSession::current()->toggle_pause();
      break;

    case MNID_RESETLEVEL:
      if (g_config->confirmation_dialog)
      {
        Dialog::show_confirmation(_("Are you sure?"), reset_callback);
      }
      else
      {
        reset_callback();
      }
      break;

    case MNID_RESETLEVELCHECKPOINT:
      if (g_config->confirmation_dialog)
      {
        Dialog::show_confirmation(_("Are you sure?"),
                                  reset_checkpoint_callback);
      }
      else
      {
        reset_checkpoint_callback();
      }
      break;

    case MNID_ABORTLEVEL:
      if (g_config->confirmation_dialog)
      {
        Dialog::show_confirmation(_("Are you sure?"), abort_callback);
      }
      else
      {
        abort_callback();
      }
      break;
  }
}

/* EOF */
