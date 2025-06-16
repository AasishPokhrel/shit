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

#include "video/gl/gl_framebuffer.hpp"

#include "video/glutil.hpp"
#include "video/gl/gl_texture.hpp"

GLFramebuffer::GLFramebuffer(/*GLVideoSystem& video_system,*/ GLTexture& texture) :
  //m_video_system(video_system),
  m_handle(0)
{
  assert_gl();

  glGenFramebuffers(1, &m_handle);

  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.get_handle(), 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  assert_gl();
}

GLFramebuffer::~GLFramebuffer()
{
  glDeleteFramebuffers(1, &m_handle);
}

/* EOF */
