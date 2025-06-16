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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_RENDERER_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_RENDERER_HPP

#include "video/renderer.hpp"

#include "video/gl/gl_painter.hpp"

class GLRenderer : public Renderer
{
public:
  explicit GLRenderer(GLVideoSystem& video_system) :
    m_video_system(video_system),
    m_painter(m_video_system, *this)
  {
  }

  virtual GLPainter& get_painter() override { return m_painter; }

protected:
  GLVideoSystem& m_video_system;
  GLPainter m_painter;

private:
  GLRenderer(const GLRenderer&) = delete;
  GLRenderer& operator=(const GLRenderer&) = delete;
};

#endif

/* EOF */
