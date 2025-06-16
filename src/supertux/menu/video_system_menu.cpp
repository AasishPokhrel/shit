//  SuperTux
//  Copyright (C) 2022 mrkubax10 <mrkubax10@onet.pl>
//                2025 Vankata453
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

#include "supertux/menu/video_system_menu.hpp"

#include <fmt/format.h>

#include "gui/dialog.hpp"
#include "gui/item_action.hpp"
#include "gui/menu_item.hpp"
#include "supertux/globals.hpp"
#include "supertux/gameconfig.hpp"
#include "util/gettext.hpp"
#include "video/video_system.hpp"

VideoSystemMenu::VideoSystemMenu()
{
  refresh();
}

void
VideoSystemMenu::refresh()
{
  clear();

  add_label(_("Select Video System"));
  add_hl();

  const auto video_systems = VideoSystem::get_available_video_systems();
  for (int i = 0; i < static_cast<int>(video_systems.size()); ++i)
  {
    const VideoSystem::Info& video_system = video_systems.at(i);
    add_entry(static_cast<int>(video_system.value), video_system.value == g_config->video ? fmt::format("[{}]", video_system.name) : video_system.name)
      .set_help(video_system.description);
  }

  add_hl();
  add_back(_("Back"));
}

void
VideoSystemMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0)
  {
    g_config->video = static_cast<VideoSystem::Enum>(item.get_id());
    Dialog::show_message(_("Restart game for the changes to take effect"), false, false, [this]
      {
        refresh();
      });
  }
}

/* EOF */
