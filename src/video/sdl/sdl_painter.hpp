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

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SDL_PAINTER_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SDL_PAINTER_HPP

#include "video/painter.hpp"

#include <optional>

class Renderer;
class SDLScreenRenderer;
class SDLVideoSystem;
struct DrawingRequest;
struct SDL_Renderer;

class SDLPainter final : public Painter
{
public:
  SDLPainter(SDLVideoSystem& video_system, Renderer& renderer, SDL_Renderer* sdl_renderer);

  virtual void draw_texture(const TextureRequest& request) override;
  virtual void draw_gradient(const GradientRequest& request) override;
  virtual void draw_filled_rect(const FillRectRequest& request) override;
  virtual void draw_inverse_ellipse(const InverseEllipseRequest& request) override;
  virtual void draw_line(const LineRequest& request) override;
  virtual void draw_triangle(const TriangleRequest& request) override;

  virtual void clear(const Color& color) override;
  virtual void get_pixel(const GetPixelRequest& request) const override;

  virtual void set_clip_rect(const Rect& rect) override;
  virtual void clear_clip_rect() override;

private:
  SDLVideoSystem& m_video_system;
  Renderer& m_renderer;
  SDL_Renderer* m_sdl_renderer;
  std::optional<SDL_Rect> m_cliprect;

private:
  SDLPainter(const SDLPainter&) = delete;
  SDLPainter& operator=(const SDLPainter&) = delete;
};

#endif

/* EOF */
