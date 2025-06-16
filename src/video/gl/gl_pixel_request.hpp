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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_PIXEL_REQUEST_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_PIXEL_REQUEST_HPP

#include <stddef.h>

#include "video/color.hpp"
#include "video/gl.hpp"

#ifndef USE_OPENGLES2

class GLPixelRequest final
{
public:
  GLPixelRequest(int width, int height);
  ~GLPixelRequest();

  void request(int x, int y);
  bool is_ready() const;
  Color get_color() const;

private:
  void get(void* buffer, size_t length) const;

private:
  GLuint m_buffer;
  int m_width;
  int m_height;
  GLintptr m_offset;
  GLsync m_sync;

private:
  GLPixelRequest(const GLPixelRequest&) = delete;
  GLPixelRequest& operator=(const GLPixelRequest&) = delete;
};

#endif

#endif

/* EOF */
