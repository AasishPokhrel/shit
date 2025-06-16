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

#ifndef HEADER_SUPERTUX_VIDEO_COMPOSITOR_HPP
#define HEADER_SUPERTUX_VIDEO_COMPOSITOR_HPP

#include <vector>
#include <memory>

#include "util/obstackpp.hpp"

class DrawingContext;
class Rect;
class VideoSystem;

class Compositor final
{
public:
  /** Debug flag to disable lighting, used in the editor */
  static bool s_render_lighting;

public:
  Compositor(VideoSystem& video_system, float time_offset);
  ~Compositor();

  void render();

  /** Create a DrawingContext, if overlay is true the context will not
      feature light rendering. This is required for contexts that
      overlap with other context (e.g. the HUD in ScreenManager) as
      otherwise their lighting would get messed up. */
  DrawingContext& make_context(bool overlay = false);

private:
  VideoSystem& m_video_system;

  /* obstack holding the memory of the drawing requests */
  obstack m_obst;

  std::vector<std::unique_ptr<DrawingContext> > m_drawing_contexts;

  float m_time_offset;

private:
  Compositor(const Compositor&) = delete;
  Compositor& operator=(const Compositor&) = delete;
};

#endif

/* EOF */
