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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SDL_VIDEO_SYSTEM_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SDL_VIDEO_SYSTEM_HPP

#include <memory>
#include <SDL.h>

#include "math/size.hpp"
#include "video/sdlbase_video_system.hpp"
#include "video/viewport.hpp"

class SDLScreenRenderer;
class SDLTextureRenderer;
class TextureManager;

class SDLVideoSystem final : public SDLBaseVideoSystem
{
public:
  SDLVideoSystem();
  ~SDLVideoSystem() override;

  virtual std::string get_name() const override;

  virtual Renderer* get_back_renderer() const override { return nullptr; }
  virtual Renderer& get_renderer() const override;
  virtual Renderer& get_lightmap() const override;

  virtual TexturePtr new_texture(const SDL_Surface& image, const Sampler& sampler) override;

  virtual const Viewport& get_viewport() const override { return m_viewport; }
  virtual void apply_config() override;
  virtual void flip() override;

  virtual void set_vsync(int mode) override;
  virtual int get_vsync() const override;

  virtual SDLSurfacePtr make_screenshot() override;

private:
  void create_window();

private:
  std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> m_sdl_renderer;
  Viewport m_viewport;
  std::unique_ptr<SDLScreenRenderer> m_renderer;
  std::unique_ptr<SDLTextureRenderer> m_lightmap;
  std::unique_ptr<TextureManager> m_texture_manager;

private:
  SDLVideoSystem(const SDLVideoSystem&) = delete;
  SDLVideoSystem& operator=(const SDLVideoSystem&) = delete;
};

#endif

/* EOF */
