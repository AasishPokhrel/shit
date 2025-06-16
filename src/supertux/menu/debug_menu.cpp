//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/menu/debug_menu.hpp"

#include <algorithm>
#include <sstream>

#include "editor/editor.hpp"
#include "gui/item_action.hpp"
#include "gui/item_stringselect.hpp"
#include "supertux/debug.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "video/texture_manager.hpp"

DebugMenu::DebugMenu() :
  next_game_speed(0)
{
  add_label(_("Debug"));
  add_hl();

  { // Game speed menu entry.
    std::vector<float> game_speed_multiplier;
    std::vector<std::string> game_speeds;
    for (int percent : {5, 10, 25, 50, 75, 100, 150, 200, 300, 400, 500, 1000, 1500, 3000})
    {
      std::ostringstream out;
      out << percent << "%";
      game_speeds.push_back(out.str());
      game_speed_multiplier.push_back(100.0f / static_cast<float>(percent));
    }

    auto it = std::find(game_speed_multiplier.begin(), game_speed_multiplier.end(), g_debug.get_game_speed_multiplier());
    if (it != game_speed_multiplier.end())
    {
      next_game_speed = static_cast<int>(it - game_speed_multiplier.begin());
    }
    else
    {
      next_game_speed = 6;
    }

    auto& item = add_string_select(MNID_GAME_SPEED, _("Game Speed"), &next_game_speed, game_speeds);
    item.set_help(_("Adjust Game Speed"));
    item.set_callback([game_speed_multiplier](int i) {
        g_debug.set_game_speed_multiplier(game_speed_multiplier[i]);
      });
  }

  add_toggle(-1, _("Show Collision Rects"), &g_debug.show_collision_rects);
  add_toggle(-1, _("Show Worldmap Path"), &g_debug.show_worldmap_path);
  add_toggle(-1, _("Show Controller"), &g_config->show_controller);
  add_toggle(-1, _("Show Framerate"), &g_config->show_fps);
  add_toggle(-1, _("Draw Redundant Frames"), &g_debug.draw_redundant_frames);
  add_toggle(-1, _("Show Player Position"), &g_config->show_player_pos);
  add_toggle(-1, _("Use Bitmap Fonts"),
             []{ return g_debug.get_use_bitmap_fonts(); },
             [](bool value){ g_debug.set_use_bitmap_fonts(value); });
  add_toggle(-1, _("Show Tile IDs in Editor Toolbox"), &g_debug.show_toolbox_tile_ids);
  add_toggle(-1, _("Hide Player HUD"), &g_debug.hide_player_hud);
  
  add_entry(_("Reload Resources"), &Resources::reload_all)
    .set_help(_("Reloads all fonts, textures, sprites and tilesets."));
    
  add_entry(_("Dump Texture Cache"), []{ TextureManager::current()->debug_print(get_logging_instance()); });

  add_hl();
  add_back(_("Back"));
}

DebugMenu::~DebugMenu()
{
  auto editor = Editor::current();

  if (editor == nullptr) return;
  editor->m_reactivate_request = true;
}

void
DebugMenu::menu_action(MenuItem& item)
{
  g_config->save();
}

/* EOF */
