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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL33CORE_CONTEXT_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL33CORE_CONTEXT_HPP

#include "video/gl/gl_context.hpp"

#include <memory>

class GLProgram;
class GLTexture;
class GLVertexArrays;
class GLVideoSystem;

class GL33CoreContext final : public GLContext
{
public:
  GL33CoreContext(GLVideoSystem& video_system);
  ~GL33CoreContext() override;

  virtual std::string get_name() const override { return "opengl33"; }

  virtual void bind() override;

  virtual void ortho(float width, float height, bool vflip) override;

  virtual void blend_func(GLenum src, GLenum dst) override;

  virtual void set_positions(const float* data, size_t size) override;

  virtual void set_texcoords(const float* data, size_t size) override;
  virtual void set_texcoord(float u, float v) override;

  virtual void set_colors(const float* data, size_t size) override;
  virtual void set_color(const Color& color) override;

  virtual void bind_texture(const Texture& texture, const Texture* displacement_texture) override;
  virtual void bind_no_texture() override;
  virtual void draw_arrays(GLenum type, GLint first, GLsizei count) override;

  virtual bool supports_framebuffer() const override { return true; }

  inline GLProgram& get_program() const { return *m_program; }
  inline GLVertexArrays& get_vertex_arrays() const { return *m_vertex_arrays; }
  inline GLTexture& get_white_texture() const { return *m_white_texture; }

private:
  GLVideoSystem& m_video_system;
  std::unique_ptr<GLProgram> m_program;
  std::unique_ptr<GLVertexArrays> m_vertex_arrays;
  std::unique_ptr<GLTexture> m_white_texture;
  std::unique_ptr<GLTexture> m_black_texture;
  std::unique_ptr<GLTexture> m_grey_texture;
  std::unique_ptr<GLTexture> m_transparent_texture;

private:
  GL33CoreContext(const GL33CoreContext&) = delete;
  GL33CoreContext& operator=(const GL33CoreContext&) = delete;
};

#endif

/* EOF */
