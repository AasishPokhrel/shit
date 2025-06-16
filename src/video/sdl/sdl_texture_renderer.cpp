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

#include "video/sdl/sdl_texture_renderer.hpp"

#include <sstream>

#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "video/drawing_request.hpp"
#include "video/sdl/sdl_painter.hpp"
#include "video/sdl/sdl_screen_renderer.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_video_system.hpp"
#include "video/video_system.hpp"

SDLTextureRenderer::SDLTextureRenderer(SDLVideoSystem& video_system, SDL_Renderer* renderer, const Size& size, int downscale) :
  m_video_system(video_system),
  m_renderer(renderer),
  m_painter(m_video_system, *this, m_renderer),
  m_size(size),
  m_downscale(downscale),
  m_texture()
{
}

SDLTextureRenderer::~SDLTextureRenderer()
{
}

SDL_Texture*
SDLTextureRenderer::get_sdl_texture() const
{
  return static_cast<SDLTexture*>(m_texture.get())->get_texture();
}

void
SDLTextureRenderer::start_draw()
{
  if (!m_texture)
  {
    const int w = m_size.width / m_downscale;
    const int h = m_size.height / m_downscale;
    SDL_Texture* sdl_texture = SDL_CreateTexture(m_renderer,
                                                 SDL_PIXELFORMAT_RGB888,
                                                 SDL_TEXTUREACCESS_TARGET,
                                                 w, h);
    if (!sdl_texture)
    {
      std::stringstream msg;
      msg << "Couldn't create lightmap texture: " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }

    m_texture = TexturePtr(new SDLTexture(sdl_texture, w, h, Sampler()));
  }

  SDL_SetRenderTarget(m_renderer, get_sdl_texture());
  SDL_RenderSetScale(m_renderer,
                     1.0f / static_cast<float>(m_downscale),
                     1.0f / static_cast<float>(m_downscale));
}

void
SDLTextureRenderer::end_draw()
{
  SDL_RenderSetScale(m_renderer, 1.0f, 1.0f);
  SDL_SetRenderTarget(m_renderer, nullptr);
}

Rect
SDLTextureRenderer::get_rect() const
{
  return Rect(0, 0,
              Size(m_size.width / m_downscale,
                   m_size.height / m_downscale));
}

Size
SDLTextureRenderer::get_logical_size() const
{
  return m_size;
}

TexturePtr
SDLTextureRenderer::get_texture() const
{
  return m_texture;
}

/* EOF */
