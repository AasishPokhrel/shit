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

#include "video/gl/gl20_context.hpp"

#include "supertux/globals.hpp"
#include "video/glutil.hpp"
#include "video/color.hpp"
#include "video/gl/gl_texture.hpp"

#ifndef USE_OPENGLES2

GL20Context::GL20Context()
{
  assert_gl();
}

GL20Context::~GL20Context()
{
}

void
GL20Context::bind()
{
  assert_gl();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);

  assert_gl();
}

void
GL20Context::ortho(float width, float height, bool vflip)
{
  assert_gl();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (vflip)
  {
    glOrtho(0, static_cast<double>(width),
            static_cast<double>(height), 0,
            -1, 1);
  }
  else
  {
    glOrtho(0, static_cast<double>(width),
            0, static_cast<double>(height),
            -1, 1);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  assert_gl();
}

void
GL20Context::blend_func(GLenum src, GLenum dst)
{
  assert_gl();

  glBlendFunc(src, dst);

  assert_gl();
}

void
GL20Context::set_positions(const float* data, size_t size)
{
  assert_gl();

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, data);

  assert_gl();
}

void
GL20Context::set_texcoords(const float* data, size_t size)
{
  assert_gl();

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, data);

  assert_gl();
}

void
GL20Context::set_texcoord(float u, float v)
{
  assert_gl();

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  assert_gl();
}

void
GL20Context::set_colors(const float* data, size_t size)
{
  assert_gl();

  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_FLOAT, 0, data);

  assert_gl();
}

void
GL20Context::set_color(const Color& color)
{
  assert_gl();

  glDisableClientState(GL_COLOR_ARRAY);
  glColor4f(color.red, color.green, color.blue, color.alpha);

  assert_gl();
}

void
GL20Context::bind_texture(const Texture& texture, const Texture* displacement_texture)
{
  assert_gl();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, static_cast<const GLTexture&>(texture).get_handle());

  assert_gl();

  Vector animate = static_cast<const GLTexture&>(texture).get_sampler().get_animate();
  if (animate.x == 0.0f && animate.y == 0.0f)
  {
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
  }
  else
  {
    animate.x /= static_cast<float>(texture.get_image_width());
    animate.y /= static_cast<float>(texture.get_image_height());

    animate *= g_game_time;

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(animate.x, animate.y, 0.0f);
    glMatrixMode(GL_MODELVIEW);
  }

  assert_gl();
}

void
GL20Context::bind_no_texture()
{
  assert_gl();

  glDisable(GL_TEXTURE_2D);

  assert_gl();
}

void
GL20Context::draw_arrays(GLenum type, GLint first, GLsizei count)
{
  assert_gl();

  glDrawArrays(type, first, count);

  assert_gl();
}

#endif

/* EOF */
