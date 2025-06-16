//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//	Updated by GiBy 2013 for SDL2 <giby_the_kid@yahoo.fr>
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

#include "video/gl/gl_screen_renderer.hpp"

#include "math/rect.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "video/gl/gl_context.hpp"
#include "video/gl/gl_program.hpp"
#include "video/gl/gl_vertex_arrays.hpp"
#include "video/gl/gl_video_system.hpp"
#include "video/glutil.hpp"

GLScreenRenderer::GLScreenRenderer(GLVideoSystem& video_system) :
  GLRenderer(video_system)
{
}

GLScreenRenderer::~GLScreenRenderer()
{
}

void
GLScreenRenderer::start_draw()
{
  assert_gl();

  GLContext& context = m_video_system.get_context();
  context.bind();

  context.blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const Viewport& viewport = m_video_system.get_viewport();
  const Rect& rect = viewport.get_rect();

  glViewport(rect.left, rect.top, rect.get_width(), rect.get_height());

  context.ortho(static_cast<float>(viewport.get_screen_width()),
                static_cast<float>(viewport.get_screen_height()),
                true);

  // Clear the screen to get rid of lightmap remains.
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  assert_gl();
}

void
GLScreenRenderer::end_draw()
{
}

Rect
GLScreenRenderer::get_rect() const
{
  const Viewport& viewport = m_video_system.get_viewport();
  return viewport.get_rect();
}

Size
GLScreenRenderer::get_logical_size() const
{
  const Viewport& viewport = m_video_system.get_viewport();
  return Size(viewport.get_screen_width(),
              viewport.get_screen_height());
}

/* EOF */
