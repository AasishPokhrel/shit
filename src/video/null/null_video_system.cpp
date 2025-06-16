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

#include "video/null/null_video_system.hpp"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "video/null/null_renderer.hpp"
#include "video/null/null_texture.hpp"
#include "video/sdl_surface_ptr.hpp"
#include "video/texture_manager.hpp"

NullVideoSystem::NullVideoSystem() :
  m_window_size(g_config->window_size),
  m_vsync_mode(0),
  m_viewport(Rect(0, 0, 1920, 1080), Vector(1.0f, 1.0f)),
  m_screen_renderer(new NullRenderer),
  m_lightmap_renderer(new NullRenderer),
  m_texture_manager(new TextureManager)
{
}

NullVideoSystem::~NullVideoSystem()
{
}

Renderer*
NullVideoSystem::get_back_renderer() const
{
  return nullptr;
}

Renderer&
NullVideoSystem::get_renderer() const
{
  return *m_screen_renderer;
}

Renderer&
NullVideoSystem::get_lightmap() const
{
  return *m_lightmap_renderer;
}

TexturePtr
NullVideoSystem::new_texture(const SDL_Surface& image, const Sampler& sampler)
{
  return TexturePtr(new NullTexture(Size(image.w, image.h)));
}

const Viewport&
NullVideoSystem::get_viewport() const
{
  return m_viewport;
}

void
NullVideoSystem::apply_config()
{
}

void
NullVideoSystem::flip()
{
  log_info << "NullVideoSystem::flip()" << std::endl;
}

void
NullVideoSystem::on_resize(int w, int h)
{
  log_info << "NullVideoSystem::on_resize(" << w << ", " << h << ")" << std::endl;
  m_window_size = Size(w, h);
}

Size
NullVideoSystem::get_window_size() const
{
  return m_window_size;
}

void
NullVideoSystem::set_vsync(int mode)
{
  log_info << "VideoSystem::set_vsync(" << mode << ")" << std::endl;
  m_vsync_mode = mode;
}

int
NullVideoSystem::get_vsync() const
{
  return m_vsync_mode;
}

void
NullVideoSystem::set_title(const std::string& title)
{
  log_info << "VideoSystem::set_icon(\"" << title << "\")" << std::endl;
}

void
NullVideoSystem::set_icon(const SDL_Surface& icon)
{
  log_info << "VideoSystem::set_icon()" << std::endl;
}

SDLSurfacePtr
NullVideoSystem::make_screenshot()
{
  return {};
}

/* EOF */
