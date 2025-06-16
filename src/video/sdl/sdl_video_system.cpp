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

#include "video/sdl/sdl_video_system.hpp"

#include <sstream>

#include "math/rect.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "video/renderer.hpp"
#include "video/sdl/sdl_screen_renderer.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_texture_renderer.hpp"
#include "video/sdl_surface.hpp"
#include "video/texture_manager.hpp"

SDLVideoSystem::SDLVideoSystem() :
  m_sdl_renderer(nullptr, &SDL_DestroyRenderer),
  m_viewport(),
  m_renderer(),
  m_lightmap(),
  m_texture_manager()
{
  create_window();

  m_renderer.reset(new SDLScreenRenderer(*this, m_sdl_renderer.get()));
  m_texture_manager.reset(new TextureManager);

  apply_config();
}

SDLVideoSystem::~SDLVideoSystem()
{
}

std::string
SDLVideoSystem::get_name() const
{
  SDL_version version;
  SDL_GetVersion(&version);
  std::ostringstream out;
  out << "SDL "
      << static_cast<int>(version.major)
      << "." << static_cast<int>(version.minor)
      << "." << static_cast<int>(version.patch);
  return out.str();
}

void
SDLVideoSystem::create_window()
{
  log_info << "Creating SDLVideoSystem" << std::endl;

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");

  create_sdl_window(0);

  m_sdl_renderer.reset(SDL_CreateRenderer(m_sdl_window.get(), -1, 0));
  if (!m_sdl_renderer)
  {
    std::stringstream msg;
    msg << "Couldn't create SDL_Renderer: " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }
}

void
SDLVideoSystem::apply_config()
{
  apply_video_mode();

  { // apply_viewport
    Size target_size = (g_config->use_fullscreen && g_config->fullscreen_size != Size(0, 0)) ?
      g_config->fullscreen_size :
      g_config->window_size;

    m_viewport = Viewport::from_size(target_size, m_desktop_size);
  }

  m_lightmap.reset(new SDLTextureRenderer(*this, m_sdl_renderer.get(), m_viewport.get_screen_size(), 5));
}

Renderer&
SDLVideoSystem::get_renderer() const
{
  return *m_renderer;
}

Renderer&
SDLVideoSystem::get_lightmap() const
{
  return *m_lightmap;
}

TexturePtr
SDLVideoSystem::new_texture(const SDL_Surface& image, const Sampler& sampler)
{
  return TexturePtr(new SDLTexture(image, sampler));
}

void
SDLVideoSystem::set_vsync(int mode)
{
  log_warning << "Setting vsync not supported by SDL renderer" << std::endl;
}

int
SDLVideoSystem::get_vsync() const
{
  return 0;
}

void
SDLVideoSystem::flip()
{
  m_renderer->flip();
}

SDLSurfacePtr
SDLVideoSystem::make_screenshot()
{
  int width;
  int height;
  if (SDL_GetRendererOutputSize(m_renderer->get_sdl_renderer(), &width, &height) != 0)
  {
    log_warning << "SDL_GetRenderOutputSize failed: " << SDL_GetError() << std::endl;
    return {};
  }
  else
  {
    SDLSurfacePtr surface = SDLSurface::create_rgba(width, height);

    SDL_LockSurface(surface.get());
    int ret = SDL_RenderReadPixels(m_renderer->get_sdl_renderer(), nullptr,
                                   SDL_PIXELFORMAT_ABGR8888,
                                   surface->pixels,
                                   surface->pitch);
    SDL_UnlockSurface(surface.get());

    if (ret != 0)
    {
      log_warning << "SDL_RenderReadPixels failed: " << SDL_GetError() << std::endl;
      return {};
    }
    else
    {
      return surface;
    }
  }
}

/* EOF */
