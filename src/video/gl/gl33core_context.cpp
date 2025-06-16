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

#include "video/gl/gl33core_context.hpp"

#include "supertux/globals.hpp"
#include "video/color.hpp"
#include "video/gl/gl_program.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/gl/gl_texture_renderer.hpp"
#include "video/gl/gl_vertex_arrays.hpp"
#include "video/gl/gl_video_system.hpp"
#include "video/glutil.hpp"

GL33CoreContext::GL33CoreContext(GLVideoSystem& video_system) :
  m_video_system(video_system),
  m_program(),
  m_vertex_arrays(),
  m_white_texture(),
  m_black_texture(),
  m_grey_texture(),
  m_transparent_texture()
{
  assert_gl();

  m_program.reset(new GLProgram);
  m_vertex_arrays.reset(new GLVertexArrays(*this));
  m_white_texture.reset(new GLTexture(1, 1, Color::WHITE));
  m_black_texture.reset(new GLTexture(1, 1, Color::BLACK));
  m_grey_texture.reset(new GLTexture(1, 1, Color::from_rgba8888(128, 128, 0, 0)));
  m_transparent_texture.reset(new GLTexture(1, 1, Color(1.0f, 0, 0, 0)));

  assert_gl();
}

GL33CoreContext::~GL33CoreContext()
{
}

void
GL33CoreContext::bind()
{
  assert_gl();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);

  m_program->bind();
  m_vertex_arrays->bind();

  GLTextureRenderer* back_renderer = static_cast<GLTextureRenderer*>(m_video_system.get_back_renderer());

  GLTexture* texture;
  if (back_renderer->is_rendering() || !back_renderer->get_texture())
  {
    texture = m_black_texture.get();
    glUniform1f(m_program->get_backbuffer_location(), 0.0f);
  }
  else
  {
    texture = static_cast<GLTexture*>(back_renderer->get_texture().get());
    glUniform1f(m_program->get_backbuffer_location(), 1.0f);
  }

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, texture->get_handle());

  const float tsx =
    static_cast<float>(texture->get_image_width()) /
    static_cast<float>(texture->get_texture_width());

  const float tsy =
    static_cast<float>(texture->get_image_height()) /
    static_cast<float>(texture->get_texture_height());

  const Rect& rect = m_video_system.get_viewport().get_rect();

  const float sx = tsx / static_cast<float>(rect.get_width());
  const float sy = tsy / static_cast<float>(rect.get_height());
  const float tx = -static_cast<float>(rect.left) / static_cast<float>(rect.get_width());
  const float ty = -static_cast<float>(rect.top) / static_cast<float>(rect.get_height());

  const float matrix[3*3] = {
    sx, 0.0, 0,
    0.0, sy, 0,
    tx, ty, 1.0,
  };
  glUniformMatrix3fv(m_program->get_fragcoord2uv_location(),
                     1, false, matrix);

  glUniform1i(m_program->get_diffuse_texture_location(), 0);
  glUniform1i(m_program->get_displacement_texture_location(), 1);
  glUniform1i(m_program->get_framebuffer_texture_location(), 2);

  glUniform1f(m_program->get_game_time_location(), g_game_time);

  assert_gl();
}

void
GL33CoreContext::ortho(float width, float height, bool vflip)
{
  assert_gl();

  const float sx = 2.0f / static_cast<float>(width);
  const float sy = -2.0f / static_cast<float>(height) * (vflip ? 1.0f : -1.0f);

  const float tx = -1.0f;
  const float ty = 1.0f * (vflip ? 1.0f : -1.0f);

  const float mvp_matrix[] = {
    sx, 0, tx,
    0, sy, ty,
    0, 0, 1
  };

  const GLint mvp_loc = m_program->get_modelviewprojection_location();
  glUniformMatrix3fv(mvp_loc, 1, false, mvp_matrix);

  assert_gl();
}

void
GL33CoreContext::blend_func(GLenum src, GLenum dst)
{
  assert_gl();

  glBlendFunc(src, dst);

  assert_gl();
}

void
GL33CoreContext::set_positions(const float* data, size_t size)
{
  m_vertex_arrays->set_positions(data, size);
}

void
GL33CoreContext::set_texcoords(const float* data, size_t size)
{
  m_vertex_arrays->set_texcoords(data, size);
}

void
GL33CoreContext::set_texcoord(float u, float v)
{
  m_vertex_arrays->set_texcoord(u, v);
}

void
GL33CoreContext::set_colors(const float* data, size_t size)
{
  m_vertex_arrays->set_colors(data, size);
}

void
GL33CoreContext::set_color(const Color& color)
{
  m_vertex_arrays->set_color(color);
}

void
GL33CoreContext::bind_texture(const Texture& texture, const Texture* displacement_texture)
{
  assert_gl();

  GLTextureRenderer* back_renderer = static_cast<GLTextureRenderer*>(m_video_system.get_back_renderer());

  if (displacement_texture && back_renderer->is_rendering())
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_transparent_texture->get_handle());
  }
  else
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, static_cast<const GLTexture&>(texture).get_handle());

    Vector animate = static_cast<const GLTexture&>(texture).get_sampler().get_animate();

    animate.x /= static_cast<float>(texture.get_image_width());
    animate.y /= static_cast<float>(texture.get_image_height());

    glUniform2f(m_program->get_animate_location(), animate.x, animate.y);
  }

  if (displacement_texture)
  {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, static_cast<const GLTexture&>(*displacement_texture).get_handle());

    Vector animate = static_cast<const GLTexture&>(*displacement_texture).get_sampler().get_animate();

    animate.x /= static_cast<float>(displacement_texture->get_image_width());
    animate.y /= static_cast<float>(displacement_texture->get_image_height());

    glUniform2f(m_program->get_displacement_animate_location(), animate.x, animate.y);
  }
  else
  {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_grey_texture->get_handle());
  }

  assert_gl();
}

void
GL33CoreContext::bind_no_texture()
{
  assert_gl();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_white_texture->get_handle());

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_grey_texture->get_handle());

  assert_gl();
}

void
GL33CoreContext::draw_arrays(GLenum type, GLint first, GLsizei count)
{
  assert_gl();

  glDrawArrays(type, first, count);

  assert_gl();
}

/* EOF */
