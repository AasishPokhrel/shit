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

#ifndef HEADER_SUPERTUX_VIDEO_NULL_NULL_VIDEO_SYSTEM_HPP
#define HEADER_SUPERTUX_VIDEO_NULL_NULL_VIDEO_SYSTEM_HPP

#include "video/viewport.hpp"
#include "video/video_system.hpp"

class TextureManager;
class NullRenderer;

/** A video system that doesn't produce any output and doesn't open a
    window. Useful for debugging, testing and automation. */
class NullVideoSystem : public VideoSystem
{
public:
  NullVideoSystem();
  ~NullVideoSystem() override;

  virtual std::string get_name() const override { return "Null"; }

  virtual Renderer* get_back_renderer() const override;
  virtual Renderer& get_renderer() const override;
  virtual Renderer& get_lightmap() const override;

  virtual TexturePtr new_texture(const SDL_Surface& image, const Sampler& sampler)  override;

  virtual const Viewport& get_viewport() const override;
  virtual void apply_config() override;
  virtual void flip() override;
  virtual void on_resize(int w, int h) override;
  virtual Size get_window_size() const override;

  virtual void set_vsync(int mode) override;
  virtual int get_vsync() const override;
  virtual void set_title(const std::string& title) override;
  virtual void set_icon(const SDL_Surface& icon) override;
  virtual SDLSurfacePtr make_screenshot() override;

private:
  Size m_window_size;
  int m_vsync_mode;
  Viewport m_viewport;
  std::unique_ptr<NullRenderer> m_screen_renderer;
  std::unique_ptr<NullRenderer> m_lightmap_renderer;
  std::unique_ptr<TextureManager> m_texture_manager;

private:
  NullVideoSystem(const NullVideoSystem&) = delete;
  NullVideoSystem& operator=(const NullVideoSystem&) = delete;
};

#endif

/* EOF */
