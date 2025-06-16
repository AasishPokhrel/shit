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

#ifndef HEADER_SUPERTUX_SUPERTUX_DEBUG_HPP
#define HEADER_SUPERTUX_SUPERTUX_DEBUG_HPP

class Debug
{
public:
  Debug();

  void set_use_bitmap_fonts(bool value);
  inline bool get_use_bitmap_fonts() const { return m_use_bitmap_fonts; }

  void set_game_speed_multiplier(float v);
  inline float get_game_speed_multiplier() const { return m_game_speed_multiplier; }

public:
  /** Show collision rectangles of moving objects */
  bool show_collision_rects;

  /** Draw the path on the worldmap, including invisible paths */
  bool show_worldmap_path;

  // Draw frames even when visually nothing changes; this can be used to
  // vaguely measure the impact of code changes which should increase the FPS
  bool draw_redundant_frames;

  /** Draw tile IDs in editor toolbox */
  bool show_toolbox_tile_ids;

  /** Do not draw PlayerStatusHUD and LevelTime */
  bool hide_player_hud;

private:
  /** Use old bitmap fonts instead of TTF */
  bool m_use_bitmap_fonts;

  /** Speed up or slow down the game */
  float m_game_speed_multiplier;

private:
  Debug(const Debug&) = delete;
  Debug& operator=(const Debug&) = delete;
};

extern Debug g_debug;

#endif

/* EOF */
