//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_VIDEO_GLUTIL_HPP
#define HEADER_SUPERTUX_VIDEO_GLUTIL_HPP

#include <sstream>
#include <stdexcept>

#include "video/gl.hpp"

#ifdef USE_GLBINDING
#  include <glbinding/ContextInfo.h>
#  include <glbinding/gl/extension.h>
#endif

inline void check_gl_error(const char* filename, int line)
{
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::ostringstream msg;
    msg << filename << ":" << line << ": " << "glGetError: ";
    switch (error) {
      case GL_INVALID_ENUM:
        msg << "INVALID_ENUM: An unacceptable value is specified for an "
          "enumerated argument.";
        break;
      case GL_INVALID_VALUE:
        msg << "INVALID_VALUE: A numeric argument is out of range.";
        break;
      case GL_INVALID_OPERATION:
        msg << "INVALID_OPERATION: The specified operation is not allowed "
          "in the current state.";
        break;
#ifdef GL_STACK_OVERFLOW
      case GL_STACK_OVERFLOW:
        msg << "STACK_OVERFLOW: This command would cause a stack overflow.";
        break;
#endif
#ifdef GL_STACK_UNDERFLOW
      case GL_STACK_UNDERFLOW:
        msg << "STACK_UNDERFLOW: This command would cause a stack underflow.";
        break;
#endif
      case GL_OUT_OF_MEMORY:
        msg << "OUT_OF_MEMORY: There is not enough memory left to execute the "
          "command.";
        break;
#ifdef GL_TABLE_TOO_LARGE
      case GL_TABLE_TOO_LARGE:
        msg << "TABLE_TOO_LARGE: table is too large";
        break;
#endif
      default:
        msg << "Unknown error (code " << error << ")";
    }

    throw std::runtime_error(msg.str());
  }
}

#define assert_gl() check_gl_error(__FILE__, __LINE__)

inline bool gl_needs_power_of_two()
{
#if defined(USE_OPENGLES2)
  return true;
#elif defined(USE_OPENGLES1)
  return true;
#else
#  ifdef USE_GLBINDING
  static auto extensions = glbinding::ContextInfo::extensions();
  return extensions.find(GLextension::GL_ARB_texture_non_power_of_two) == extensions.end();
#  else
  return !GLEW_ARB_texture_non_power_of_two;
#  endif
#endif
}

inline bool is_power_of_2(int v)
{
  return (v & (v-1)) == 0;
}

inline int next_power_of_two(int val)
{
  int result = 1;
  while (result < val)
    result *= 2;
  return result;
}

#endif

/* EOF */
