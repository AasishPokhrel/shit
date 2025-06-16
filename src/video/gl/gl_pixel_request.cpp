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

#include "video/gl/gl_pixel_request.hpp"

#include <iostream>

#include "util/log.hpp"
#include "video/glutil.hpp"

#ifndef USE_OPENGLES2

GLPixelRequest::GLPixelRequest(int width, int height) :
  m_buffer(),
  m_width(width),
  m_height(height),
  m_offset(0),
  m_sync()
{
  assert_gl();

  glGenBuffers(1, &m_buffer);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
  glBufferData(GL_PIXEL_PACK_BUFFER, m_width * m_height * 4, nullptr, GL_DYNAMIC_READ);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

  assert_gl();
}

GLPixelRequest::~GLPixelRequest()
{
  glDeleteBuffers(1, &m_buffer);
}

void
GLPixelRequest::request(int x, int y)
{
  assert_gl();

  glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
  glReadPixels(x, y, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE,
               reinterpret_cast<GLvoid*>(m_offset));
  m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, GL_NONE_BIT);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

  assert_gl();
}

bool
GLPixelRequest::is_ready() const
{
  assert_gl();

  GLenum ret = glClientWaitSync(m_sync, GL_NONE_BIT, 0);

  if (ret == GL_CONDITION_SATISFIED ||
      ret == GL_ALREADY_SIGNALED)
  {
    //std::cout << "fence ready: " << static_cast<int>(ret) << " " << GL_ALREADY_SIGNALED << std::endl;
    return true;
  }
  else if (ret == GL_TIMEOUT_EXPIRED)

  {
    //std::cout << "timeout: " << static_cast<int>(ret) << std::endl;
    return false;
  }
  else if (ret == GL_WAIT_FAILED)
  {
    return true;
  }
  else
  {
    log_warning << "unknown glClientWaitSync() return value: " << static_cast<int>(ret) << std::endl;
    return true;
  }

  assert_gl();
}

void
GLPixelRequest::get(void* buffer, size_t length) const
{
  assert_gl();

  glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
  glGetBufferSubData(GL_PIXEL_PACK_BUFFER, m_offset, length, buffer);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

  assert_gl();
}

Color
GLPixelRequest::get_color() const
{
  uint8_t data[3];
  get(data, sizeof(data));
  return Color::from_rgb888(data[0], data[1], data[2]);
}

#endif

/* EOF */
