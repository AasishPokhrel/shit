//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//	Updated by GiBy 2013 for SDL2 <giby_the_kid@yahoo.fr>
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

#include "video/sdl/sdl_screen_renderer.hpp"

#include "math/rect.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "video/sdl/sdl_painter.hpp"
#include "video/sdl/sdl_video_system.hpp"

SDLScreenRenderer::SDLScreenRenderer(SDLVideoSystem& video_system, SDL_Renderer* renderer) :
  m_video_system(video_system),
  m_renderer(renderer),
  m_painter(m_video_system, *this, m_renderer)
{
  SDL_RendererInfo info;
  if (SDL_GetRendererInfo(m_renderer, &info) != 0)
  {
    log_warning << "Couldn't get RendererInfo: " << SDL_GetError() << std::endl;
  }
  else
  {
    log_info << "SDL_Renderer: " << info.name << std::endl;
    log_info << "SDL_RendererFlags: " << std::endl;
    if (info.flags & SDL_RENDERER_SOFTWARE) { log_info << "  SDL_RENDERER_SOFTWARE" << std::endl; }
    if (info.flags & SDL_RENDERER_ACCELERATED) { log_info << "  SDL_RENDERER_ACCELERATED" << std::endl; }
    if (info.flags & SDL_RENDERER_PRESENTVSYNC) { log_info << "  SDL_RENDERER_PRESENTVSYNC" << std::endl; }
    if (info.flags & SDL_RENDERER_TARGETTEXTURE) { log_info << "  SDL_RENDERER_TARGETTEXTURE" << std::endl; }
    log_info << "Texture Formats: " << std::endl;
    for (size_t i = 0; i < info.num_texture_formats; ++i)
    {
      log_info << "  " << SDL_GetPixelFormatName(info.texture_formats[i]) << std::endl;
    }
    log_info << "Max Texture Width: " << info.max_texture_width << std::endl;
    log_info << "Max Texture Height: " << info.max_texture_height << std::endl;
  }
}

SDLScreenRenderer::~SDLScreenRenderer()
{
}

void
SDLScreenRenderer::start_draw()
{
  const Rect& viewport = m_video_system.get_viewport().get_rect();
  const Vector& scale = m_video_system.get_viewport().get_scale();

  SDL_Rect sdl_viewport = viewport.to_sdl();

  // SetViewport() works in scaled screen coordinates, so we have to
  // reset it to 1.0, 1.0 to get meaningful results
  SDL_RenderSetScale(m_renderer, 1.0f, 1.0f);
  SDL_RenderSetViewport(m_renderer, &sdl_viewport);
  SDL_RenderSetScale(m_renderer, scale.x, scale.y);

  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);
}

void
SDLScreenRenderer::end_draw()
{
}

Rect
SDLScreenRenderer::get_rect() const
{
  return m_video_system.get_viewport().get_rect();
}

Size
SDLScreenRenderer::get_logical_size() const
{
  return m_video_system.get_viewport().get_screen_size();
}

void
SDLScreenRenderer::flip()
{
  SDL_RenderPresent(m_renderer);
}

/* EOF */
