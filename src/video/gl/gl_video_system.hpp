//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_VIDEO_SYSTEM_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_VIDEO_SYSTEM_HPP

#include <memory>
#include <SDL.h>

#include "math/size.hpp"
#include "video/sdlbase_video_system.hpp"
#include "video/viewport.hpp"

class GLContext;
class GLLightmap;
class GLProgram;
class GLScreenRenderer;
class GLTexture;
class GLTextureRenderer;
class GLVertexArrays;
class Rect;
class TextureManager;
struct SDL_Surface;

class GLVideoSystem final : public SDLBaseVideoSystem
{
public:
  GLVideoSystem(bool use_opengl33core, bool auto_opengl_version);
  ~GLVideoSystem() override;

  virtual std::string get_name() const override;

  virtual Renderer* get_back_renderer() const override;
  virtual Renderer& get_renderer() const override;
  virtual Renderer& get_lightmap() const override;

  virtual TexturePtr new_texture(const SDL_Surface& image, const Sampler& sampler) override;

  virtual const Viewport& get_viewport() const override { return m_viewport; }
  virtual void apply_config() override;
  virtual void flip() override;

  virtual void set_vsync(int mode) override;
  virtual int get_vsync() const override;

  virtual SDLSurfacePtr make_screenshot() override;

  inline GLContext& get_context() const { return *m_context; }

private:
  void create_gl_window();
  void create_gl_context();

private:
  bool m_use_opengl33core;
  std::unique_ptr<TextureManager> m_texture_manager;
  std::unique_ptr<GLScreenRenderer> m_renderer;
  std::unique_ptr<GLTextureRenderer> m_lightmap;
  std::unique_ptr<GLTextureRenderer> m_back_renderer;
  std::unique_ptr<GLContext> m_context;

  SDL_GLContext m_glcontext;
  Viewport m_viewport;

private:
  GLVideoSystem(const GLVideoSystem&) = delete;
  GLVideoSystem& operator=(const GLVideoSystem&) = delete;
};

#endif

/* EOF */
