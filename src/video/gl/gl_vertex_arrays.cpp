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

#include "video/gl/gl_vertex_arrays.hpp"

#include "video/color.hpp"
#include "video/gl/gl33core_context.hpp"
#include "video/gl/gl_program.hpp"
#include "video/gl/gl_video_system.hpp"
#include "video/glutil.hpp"

GLVertexArrays::GLVertexArrays(GL33CoreContext& context) :
  m_context(context),
  m_vao(),
  m_positions_buffer(),
  m_texcoords_buffer(),
  m_color_buffer()
{
  assert_gl();

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_positions_buffer);
  glGenBuffers(1, &m_texcoords_buffer);
  glGenBuffers(1, &m_color_buffer);

  assert_gl();
}

GLVertexArrays::~GLVertexArrays()
{
  glDeleteBuffers(1, &m_positions_buffer);
  glDeleteBuffers(1, &m_texcoords_buffer);
  glDeleteBuffers(1, &m_color_buffer);
  glDeleteVertexArrays(1, &m_vao);
}

void
GLVertexArrays::bind()
{
  assert_gl();

  glBindVertexArray(m_vao);

  assert_gl();
}

void
GLVertexArrays::set_positions(const float* data, size_t size)
{
  assert_gl();

  glBindBuffer(GL_ARRAY_BUFFER, m_positions_buffer);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

  int loc = m_context.get_program().get_position_location();
  glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(loc);

  assert_gl();
}

void
GLVertexArrays::set_texcoords(const float* data, size_t size)
{
  assert_gl();

  glBindBuffer(GL_ARRAY_BUFFER, m_texcoords_buffer);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

  int loc = m_context.get_program().get_texcoord_location();
  glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(loc);

  assert_gl();
}

void
GLVertexArrays::set_texcoord(float u, float v)
{
  assert_gl();

  int loc = m_context.get_program().get_texcoord_location();
  glVertexAttrib2f(loc, u, v);
  glDisableVertexAttribArray(loc);

  assert_gl();
}

void
GLVertexArrays::set_colors(const float* data, size_t size)
{
  assert_gl();

  glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

  int loc = m_context.get_program().get_diffuse_location();
  glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(loc);

  assert_gl();
}

void
GLVertexArrays::set_color(const Color& color)
{
  assert_gl();

  int loc = m_context.get_program().get_diffuse_location();
  glVertexAttrib4f(loc, color.red, color.green, color.blue, color.alpha);
  glDisableVertexAttribArray(loc);

  assert_gl();
}

/* EOF */
