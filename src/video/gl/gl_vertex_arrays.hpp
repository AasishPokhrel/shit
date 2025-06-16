//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_VERTEX_ARRAYS_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_VERTEX_ARRAYS_HPP

#include <stddef.h>

#include "video/gl.hpp"

class Color;
class GL33CoreContext;

class GLVertexArrays final
{
public:
  GLVertexArrays(GL33CoreContext& context);
  ~GLVertexArrays();

  void bind();

  void set_positions(const float* data, size_t size);

  /** size is in bytes */
  void set_texcoords(const float* data, size_t size);
  void set_texcoord(float u, float v);

  void set_colors(const float* data, size_t size);
  void set_color(const Color& color);

private:
  GL33CoreContext& m_context;
  GLuint m_vao;
  GLuint m_positions_buffer;
  GLuint m_texcoords_buffer;
  GLuint m_color_buffer;

private:
  GLVertexArrays(const GLVertexArrays&) = delete;
  GLVertexArrays& operator=(const GLVertexArrays&) = delete;
};

#endif

/* EOF */
