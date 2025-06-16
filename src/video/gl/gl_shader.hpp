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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_SHADER_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_SHADER_HPP

#include <memory>
#include <string>
#include <vector>

#include "video/gl.hpp"

class GLShader final
{
public:
  static std::unique_ptr<GLShader> from_file(GLenum type, const std::string& filename);
  static std::unique_ptr<GLShader> from_source(GLenum type, const std::vector<std::string>& sources);

public:
  GLShader(GLenum type);
  ~GLShader();

  GLuint get_handle() const {
    return m_shader;
  }

  std::string get_shader_info_log() const;

private:
  void source(std::vector<std::string> const& sources);
  void compile();

private:
  GLuint m_shader;

private:
  GLShader(const GLShader&) = delete;
  GLShader& operator=(const GLShader&) = delete;
};

#endif

/* EOF */
