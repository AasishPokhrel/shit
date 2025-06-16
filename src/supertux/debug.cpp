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

#include "supertux/debug.hpp"

#include "supertux/resources.hpp"
#include "util/log.hpp"

Debug g_debug;

Debug::Debug() :
  show_collision_rects(false),
  show_worldmap_path(false),
  draw_redundant_frames(false),
  show_toolbox_tile_ids(false),
  hide_player_hud(false),
  m_use_bitmap_fonts(false),
  m_game_speed_multiplier(1.0f)
{
}

void
Debug::set_use_bitmap_fonts(bool value)
{
  m_use_bitmap_fonts = value;
  Resources::load();
}

void
Debug::set_game_speed_multiplier(float v)
{
  m_game_speed_multiplier = v;
  log_info << m_game_speed_multiplier << std::endl;
}

/* EOF */
