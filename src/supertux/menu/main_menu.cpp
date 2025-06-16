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

#include "supertux/menu/main_menu.hpp"

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "gui/dialog.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/level_parser.hpp"
#include "supertux/levelset.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/world.hpp"
#include "util/log.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

#if defined(_WIN32)
  #include <windows.h>
  #include <shellapi.h>
#else
  #include <cstdlib>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

bool MainMenu::s_shown_initial_dialogs = false;

MainMenu::MainMenu()
{
  add_submenu(_("Start Game"), MenuStorage::WORLDSET_MENU);
  // TODO: Manage to build OpenSSL for Emscripten so we can build CURL so we can
  //       build the add-ons so we can re-enable them.
  //       Also see src/addon/downloader.*pp
  add_submenu(_("Add-ons"), MenuStorage::ADDON_MENU);
#ifdef __EMSCRIPTEN__
  add_submenu(_("Manage Assets"), MenuStorage::ASSET_MENU);
#endif
  add_submenu(_("Options"), MenuStorage::OPTIONS_MENU);
  add_entry(MNID_LEVELEDITOR, _("Level Editor"));
  add_entry(MNID_CREDITS, _("Credits"));
#ifndef STEAM_BUILD
  // Links to external purchases are not allowed on Steam, including donations
  add_entry(MNID_DONATE, _("Donate"));
#endif
#ifndef REMOVE_QUIT_BUTTON
  add_entry(MNID_QUITMAINMENU, _("Quit"));
#endif

  on_window_resize();

#ifndef __EMSCRIPTEN__
  // Show network-related confirmation dialogs on first startup
  if (g_config->is_initial() && !s_shown_initial_dialogs)
  {
    s_shown_initial_dialogs = true;
    Dialog::show_confirmation(_("Would you allow SuperTux to connect to the Internet?\n\nThis enables additional features, such as the in-game add-on catalog."),
      []()
      {
        g_config->disable_network = false;

        Dialog::show_confirmation(_("Would you allow SuperTux to check for new releases on startup?\n\nYou will be notified if any are found."),
          []()
          {
            g_config->do_release_check = true;
          });
      }, true);
  }
#endif
}

void
MainMenu::on_window_resize()
{
  set_center_pos(static_cast<float>(SCREEN_WIDTH) / 2.0f,
                 static_cast<float>(SCREEN_HEIGHT) / 2.0f + 35.0f);
}

void
MainMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_CREDITS:
    {
      SoundManager::current()->stop_music(0.2f);
      std::unique_ptr<World> world = World::from_directory("levels/misc");
      GameManager::current()->start_level(*world, "credits.stl"); // Credits Level
      break;
    }

    case MNID_LEVELEDITOR:
    {
      MenuManager::instance().clear_menu_stack();
      std::unique_ptr<Screen> screen(new Editor());
      auto fade = std::make_unique<FadeToBlack>(FadeToBlack::FADEOUT, 0.5f);
      SoundManager::current()->stop_music(0.5);
      ScreenManager::current()->push_screen(std::move(screen), std::move(fade));
      //Editor::current()->setup();
      break;
    }

    case MNID_DONATE:
    {
      Dialog::show_confirmation(_("This will take you to the SuperTux donation page. Are you sure you want to continue?"), [] {
        FileSystem::open_url("https://www.supertux.org/donate.html");
      });
      break;
    }

    case MNID_QUITMAINMENU:
    {
      MenuManager::instance().clear_menu_stack();
      ScreenManager::current()->quit(std::unique_ptr<ScreenFade>(new FadeToBlack(FadeToBlack::FADEOUT, 0.25f)));
      SoundManager::current()->stop_music(0.25);
      break;
    }
  }
}

/* EOF */
