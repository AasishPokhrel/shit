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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SDL_TEXTURE_RENDERER_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SDL_TEXTURE_RENDERER_HPP

#include <SDL.h>
#include <optional>

#include "video/texture_ptr.hpp"
#include "video/renderer.hpp"
#include "video/sdl/sdl_painter.hpp"

class Color;
class SDLTexture;
class SDLVideoSystem;
struct DrawingRequest;
struct SDL_Renderer;
struct SDL_Texture;

class SDLTextureRenderer final : public Renderer
{
public:
  SDLTextureRenderer(SDLVideoSystem& video_system, SDL_Renderer* renderer, const Size& size, int downscale);
  ~SDLTextureRenderer() override;

  virtual void start_draw() override;
  virtual void end_draw() override;

  virtual SDLPainter& get_painter() override { return m_painter; }

  virtual Rect get_rect() const override;
  virtual Size get_logical_size() const override;

  virtual TexturePtr get_texture() const override;

private:
  SDL_Texture* get_sdl_texture() const;

private:
  SDLVideoSystem& m_video_system;
  SDL_Renderer* m_renderer;
  SDLPainter m_painter;
  Size m_size;
  int m_downscale;

  TexturePtr m_texture;

private:
  SDLTextureRenderer(const SDLTextureRenderer&) = delete;
  SDLTextureRenderer& operator=(const SDLTextureRenderer&) = delete;
};

#endif

/* EOF */
