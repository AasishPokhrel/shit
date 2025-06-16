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

#include "supertux/menu/menu_storage.hpp"

#include <assert.h>

#include "supertux/menu/addon_menu.hpp"
#include "supertux/menu/addon_browse_menu.hpp"
#include "supertux/menu/cheat_menu.hpp"
#include "supertux/menu/debug_menu.hpp"
#include "supertux/menu/contrib_menu.hpp"
#include "supertux/menu/custom_menu_menu.hpp"
#include "supertux/menu/editor_converters_menu.hpp"
#include "supertux/menu/editor_menu.hpp"
#include "supertux/menu/editor_level_menu.hpp"
#include "supertux/menu/editor_level_select_menu.hpp"
#include "supertux/menu/editor_levelset_select_menu.hpp"
#include "supertux/menu/editor_new_levelset_menu.hpp"
#include "supertux/menu/editor_objectgroup_menu.hpp"
#include "supertux/menu/editor_tilegroup_menu.hpp"
#include "supertux/menu/editor_sector_menu.hpp"
#include "supertux/menu/editor_sectors_menu.hpp"
#include "supertux/menu/game_menu.hpp"
#include "supertux/menu/integrations_menu.hpp"
#include "supertux/menu/joystick_menu.hpp"
#include "supertux/menu/keyboard_menu.hpp"
#include "supertux/menu/language_menu.hpp"
#include "supertux/menu/main_menu.hpp"
#include "supertux/menu/multiplayer_menu.hpp"
#include "supertux/menu/multiplayer_players_menu.hpp"
#include "supertux/menu/options_select_menu.hpp"
#include "supertux/menu/particle_editor_menu.hpp"
#include "supertux/menu/particle_editor_save_as.hpp"
#include "supertux/menu/particle_editor_open.hpp"
#include "supertux/menu/profile_menu.hpp"
#include "supertux/menu/video_system_menu.hpp"
#include "supertux/menu/web_asset_menu.hpp"
#include "supertux/menu/worldmap_menu.hpp"
#include "supertux/menu/worldmap_cheat_menu.hpp"
#include "supertux/menu/world_set_menu.hpp"
#include "util/log.hpp"

MenuStorage* MenuStorage::s_instance = nullptr;

MenuStorage&
MenuStorage::instance()
{
  assert(s_instance);
  return *s_instance;
}

MenuStorage::MenuStorage()
{
  assert(!s_instance);
  s_instance = this;
}

MenuStorage::~MenuStorage()
{
  s_instance = nullptr;
}

std::unique_ptr<Menu>
MenuStorage::create(MenuId menu_id)
{
  switch (menu_id)
  {
    case MAIN_MENU:
      return std::make_unique<MainMenu>();

    case LANGUAGE_MENU:
      return std::make_unique<LanguageMenu>();

    case OPTIONS_MENU:
      return std::make_unique<OptionsSelectMenu>(true);

    case INGAME_OPTIONS_MENU:
      return std::make_unique<OptionsSelectMenu>(false);

    case PROFILE_MENU:
      return std::make_unique<ProfileMenu>();

    case KEYBOARD_MENU:
      return std::unique_ptr<Menu>(new KeyboardMenu(*InputManager::current()));

    case JOYSTICK_MENU:
      return std::unique_ptr<Menu>(new JoystickMenu(*InputManager::current()));

    case VIDEO_SYSTEM_MENU:
      return std::make_unique<VideoSystemMenu>();

    case WORLDMAP_MENU:
      return std::make_unique<WorldmapMenu>();

    case WORLDMAP_CHEAT_MENU:
      return std::make_unique<WorldmapCheatMenu>();

    case WORLDMAP_LEVEL_SELECT_MENU:
      return std::make_unique<WorldmapLevelSelectMenu>();

    case GAME_MENU:
      return std::make_unique<GameMenu>();

    case CHEAT_MENU:
      return std::make_unique<CheatMenu>();

    case DEBUG_MENU:
      return std::make_unique<DebugMenu>();

    case WORLDSET_MENU:
      return std::make_unique<WorldSetMenu>();

    case CONTRIB_MENU:
      return std::make_unique<ContribMenu>();

    case CONTRIB_WORLD_MENU:
      return nullptr; //return new ContribWorldMenu();

    case ADDON_MENU:
      return std::make_unique<AddonMenu>();

    case LANGPACK_MENU:
      return std::unique_ptr<Menu>(new AddonMenu(true));

    case EDITOR_LEVELSET_SELECT_MENU:
      return std::make_unique<EditorLevelsetSelectMenu>();

    case EDITOR_NEW_LEVELSET_MENU:
      return std::make_unique<EditorNewLevelsetMenu>();

    case LANGPACK_AUTO_UPDATE_MENU:
      return std::unique_ptr<Menu>(new AddonBrowseMenu(true, true));

    case EDITOR_LEVEL_SELECT_MENU:
      return std::make_unique<EditorLevelSelectMenu>();

    case EDITOR_MENU:
      return std::make_unique<EditorMenu>();

    case EDITOR_TILEGROUP_MENU:
      return std::make_unique<EditorTilegroupMenu>();

    case EDITOR_OBJECTGROUP_MENU:
      return std::make_unique<EditorObjectgroupMenu>();

    case EDITOR_SECTORS_MENU:
      return std::make_unique<EditorSectorsMenu>();

    case EDITOR_SECTOR_MENU:
      return std::make_unique<EditorSectorMenu>();

    case EDITOR_LEVEL_MENU:
      return std::make_unique<EditorLevelMenu>();

    case EDITOR_CONVERTERS_MENU:
      return std::make_unique<EditorConvertersMenu>();

    case INTEGRATIONS_MENU:
      return std::make_unique<IntegrationsMenu>();

    case PARTICLE_EDITOR_MENU:
      return std::make_unique<ParticleEditorMenu>();

    case PARTICLE_EDITOR_SAVE_AS:
      throw std::runtime_error("Cannot instantiate ParticleEditorSaveAs dialog "
        "from MenuStorage::create() or MenuManager::set_menu(), "
        "as it needs to be bound to a callback. "
        "Please instantiate ParticleEditorSaveAs directly");
      //return std::make_unique<ParticleEditorSaveAs>();

    case PARTICLE_EDITOR_OPEN:
      return std::make_unique<ParticleEditorOpen>();

    case ASSET_MENU:
      return std::make_unique<WebAssetMenu>();

    case CUSTOM_MENU_MENU:
      return std::make_unique<CustomMenuMenu>();

    case MULTIPLAYER_MENU:
      return std::make_unique<MultiplayerMenu>();

    case MULTIPLAYER_PLAYERS_MENU:
      return std::make_unique<MultiplayerPlayersMenu>();

    case NO_MENU:
      return std::unique_ptr<Menu>();

    default:
      log_warning << "unknown MenuId provided" << std::endl;
      assert(false);
      return std::unique_ptr<Menu>();
  }
}

/* EOF */
