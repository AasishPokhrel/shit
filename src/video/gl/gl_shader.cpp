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

#include "video/gl/gl_shader.hpp"

#include <fstream>
#include <sstream>

#include "physfs/ifile_stream.hpp"
#include "video/glutil.hpp"

std::unique_ptr<GLShader>
GLShader::from_file(GLenum type, const std::string& filename)
{
  IFileStream in(filename);
  std::vector<std::string> sources;

  std::string line;
  while (std::getline(in, line))
  {
    sources.push_back(line + "\n");
  }

  try
  {
    return from_source(type, sources);
  }
  catch(const std::exception& err)
  {
    std::ostringstream out;
    out << filename << ": " << err.what();
    throw std::runtime_error(out.str());
  }
}

std::unique_ptr<GLShader>
GLShader::from_source(GLenum type, const std::vector<std::string>& sources)
{
  assert_gl();

  auto shader = std::make_unique<GLShader>(type);
  shader->source(sources);
  shader->compile();

  assert_gl();

  return shader;
}

GLShader::GLShader(GLenum type) :
  m_shader(glCreateShader(type))
{
}

GLShader::~GLShader()
{
  glDeleteShader(m_shader);
}

void
GLShader::compile()
{
  assert_gl();

  glCompileShader(m_shader);

  GLint compile_status;
  glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compile_status);
  if (!compile_status)
  {
    std::ostringstream out;
    out << "Shader compile failed: " << get_shader_info_log() << std::endl;
    throw std::runtime_error(out.str());
  }

  assert_gl();
}

void
GLShader::source(std::vector<std::string> const& sources)
{
  assert_gl();

  std::vector<GLint> length_lst(sources.size());
  std::vector<const char*> source_lst(sources.size());
  for (size_t i = 0; i < sources.size(); ++i)
  {
    source_lst[i] = sources[i].c_str();
    length_lst[i] = static_cast<GLint>(sources[i].size());
  }

  glShaderSource(m_shader, static_cast<GLsizei>(sources.size()), source_lst.data(),
                 length_lst.data());

  assert_gl();
}

std::string
GLShader::get_shader_info_log() const
{
  assert_gl();

  GLint length = 0;
  glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &length);

  assert_gl();

  if (length == 0)
  {
    return {};
  }
  else
  {
    GLsizei out_length;
    std::vector<char> str(length);
    glGetShaderInfoLog(m_shader, static_cast<GLsizei>(str.size()), &out_length, str.data());
    return std::string(str.begin(), str.end());
  }
}

/* EOF */
