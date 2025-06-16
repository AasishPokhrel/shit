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

#include "video/sdl/sdl_painter.hpp"

#include <SDL.h>
#include <algorithm>
#include <array>
#include <assert.h>
#include <math.h>

#include "supertux/globals.hpp"
#include "math/util.hpp"
#include "util/log.hpp"
#include "video/drawing_request.hpp"
#include "video/renderer.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_video_system.hpp"
#include "video/viewport.hpp"

namespace {

SDL_BlendMode blend2sdl(const Blend& blend)
{
  if (blend == Blend::NONE)
  {
    return SDL_BLENDMODE_NONE;
  }
  else if (blend == Blend::BLEND)
  {
    return SDL_BLENDMODE_BLEND;
  }
  else if (blend == Blend::ADD)
  {
    return SDL_BLENDMODE_ADD;
  }
  else if (blend == Blend::MOD)
  {
    return SDL_BLENDMODE_MOD;
  }
  else
  {
    log_warning << "unknown blend mode combinations: blend=" << static_cast<int>(blend) << std::endl;
    return SDL_BLENDMODE_BLEND;
  }
}

/* Creates a new rectangle covering the area where srcrect and imgrect
   overlap, in addition create four more rectangles for the areas
   where srcrect is outside of imgrect, some of those rects will be
   empty. The rectangles will be returned in the order inside, top,
   left, right, bottom */
std::tuple<Rectf, Rectf, Rectf, Rectf, Rectf>
intersect(const Rect& srcrect, const Rectf& imgrect)
{
  const Rectf src_rectf = srcrect.to_rectf();

  return std::make_tuple(
    // inside
    Rectf(std::max(src_rectf.get_left(), imgrect.get_left()), std::max(src_rectf.get_top(), imgrect.get_top()),
          std::min(src_rectf.get_right(), imgrect.get_right()), std::min(src_rectf.get_bottom(), imgrect.get_bottom())),

    // top
    Rectf(src_rectf.get_left(), src_rectf.get_top(),
          src_rectf.get_right(), imgrect.get_top()),

    // left
    Rectf(src_rectf.get_left(), std::max(src_rectf.get_top(), imgrect.get_top()),
          imgrect.get_left(), std::min(src_rectf.get_bottom(), imgrect.get_bottom())),

    // right
    Rectf(imgrect.get_right(), std::max(src_rectf.get_top(), imgrect.get_top()),
          src_rectf.get_right(), std::min(src_rectf.get_bottom(), imgrect.get_bottom())),

    // bottom
    Rectf(src_rectf.get_left(), imgrect.get_bottom(),
          src_rectf.get_right(), src_rectf.get_bottom())
    );
}

/* Map the area covered by inside in srcrect to dstrect */
Rectf relative_map(const Rectf& inside, const Rect& srcrect, const Rectf& dstrect)
{
  assert(srcrect.contains(inside.to_rect()));

  const Rectf src_rectf = srcrect.to_rectf();

  Rectf result(dstrect.get_left() + (inside.get_left() - src_rectf.get_left()) * dstrect.get_width() / src_rectf.get_width(),
               dstrect.get_top() + (inside.get_top() - src_rectf.get_top()) * dstrect.get_height() / src_rectf.get_height(),
               dstrect.get_left() + (inside.get_right() - src_rectf.get_left()) * dstrect.get_width() / src_rectf.get_width(),
               dstrect.get_top() + (inside.get_bottom() - src_rectf.get_top()) * dstrect.get_height() / src_rectf.get_height());

  assert(dstrect.overlaps(result));

  return result;
}

void render_texture(SDL_Renderer* renderer,
                    SDL_Texture* texture, const Rectf& imgrect,
                    const Rect& srcrect, const Rectf& dstrect)
{
  assert(imgrect.contains(Vector(srcrect.get_left(), srcrect.get_top())));

  if (srcrect.empty() || dstrect.empty())
    return;

  if (imgrect.overlaps(srcrect))
  {
    SDL_Rect sdl_srcrect = srcrect.to_sdl();
    SDL_FRect sdl_dstrect = dstrect.to_sdl();
    SDL_RenderCopyF(renderer, texture, &sdl_srcrect, &sdl_dstrect);
  }
  else
  {
    Rectf inside;
    std::array<Rectf, 4> rest;
    std::tie(inside, rest[0], rest[1], rest[2], rest[3]) = intersect(srcrect, imgrect);

    render_texture(renderer, texture, imgrect, inside.to_rect(), relative_map(inside, srcrect, dstrect));

    for (const Rectf& rectf : rest)
    {
      const Rect rect = rectf.to_rect();
      const Rect new_srcrect(math::positive_mod(rect.get_left(), static_cast<int>(imgrect.get_width())),
                             math::positive_mod(rect.get_top(), static_cast<int>(imgrect.get_height())),
                             Size(rect.get_width(), rect.get_height()));
      render_texture(renderer, texture, imgrect,
                     new_srcrect, relative_map(rectf, srcrect, dstrect));
    }
  }
}

/* A version SDL_RenderCopyEx that supports texture animation as specified by Sampler */
void RenderCopyEx(SDL_Renderer*          renderer,
                  SDL_Texture*           texture,
                  const SDL_Rect*        sdl_srcrect,
                  const SDL_FRect*       sdl_dstrect,
                  const double           angle,
                  const SDL_Point*       center,
                  const SDL_RendererFlip flip,
                  const Sampler& sampler)
{
  Vector animate = sampler.get_animate();
  if (animate.x == 0.0f && animate.y == 0.0f)
  {
    SDL_RenderCopyExF(renderer, texture, sdl_srcrect, sdl_dstrect, angle, nullptr, flip);
  }
  else
  {
    // This part deals with texture animation. Texture animation is
    // accomplished by shifting the srcrect across the input texture.
    // If the srcrect goes out of bounds of the texture, it is broken
    // up into multiple rectangles that wrap around and fall back into
    // the texture space.
    //
    // If a srcrect is passed to SDL that goes out of bounds SDL will
    // clip it to be inside the bounds, without adjusting dstrect,
    // thus result in stretching artifacts.
    //
    // FIXME: Neither flipping nor wrap modes are supported at the
    // moment. wrap is treated as if it was set to 'repeat'.
    int width;
    int height;

    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

    animate *= g_game_time;

    int tex_off_x = math::positive_mod(static_cast<int>(animate.x), width);
    int tex_off_y = math::positive_mod(static_cast<int>(animate.y), height);

    if ((tex_off_x == 0 && tex_off_y == 0) ||
        flip ||
        angle != 0.0)
    {
      SDL_RenderCopyExF(renderer, texture, sdl_srcrect, sdl_dstrect, angle, nullptr, flip);
    }
    else
    {
      Rectf imgrect(Vector(), Sizef(static_cast<float>(width), static_cast<float>(height)));
      Rect srcrect(math::positive_mod(sdl_srcrect->x + tex_off_x, width),
                   math::positive_mod(sdl_srcrect->y + tex_off_y, height),
                   Size(sdl_srcrect->w, sdl_srcrect->h));

      render_texture(renderer, texture, imgrect, srcrect, Rectf(*sdl_dstrect));
    }
  }
}

} // namespace

SDLPainter::SDLPainter(SDLVideoSystem& video_system, Renderer& renderer, SDL_Renderer* sdl_renderer) :
  m_video_system(video_system),
  m_renderer(renderer),
  m_sdl_renderer(sdl_renderer),
  m_cliprect()
{}

void
SDLPainter::draw_texture(const TextureRequest& request)
{
  const auto& texture = static_cast<const SDLTexture&>(*request.texture);

  assert(request.srcrects.size() == request.dstrects.size());
  assert(request.srcrects.size() == request.angles.size());

  for (size_t i = 0; i < request.srcrects.size(); ++i)
  {
    const SDL_Rect& src_rect = request.srcrects[i].to_rect().to_sdl();
    const SDL_FRect& dst_rect = request.dstrects[i].to_sdl();

    Uint8 r = static_cast<Uint8>(request.color.red * 255);
    Uint8 g = static_cast<Uint8>(request.color.green * 255);
    Uint8 b = static_cast<Uint8>(request.color.blue * 255);
    Uint8 a = static_cast<Uint8>(request.color.alpha * request.alpha * 255);

    SDL_SetTextureColorMod(texture.get_texture(), r, g, b);
    SDL_SetTextureAlphaMod(texture.get_texture(), a);
    SDL_SetTextureBlendMode(texture.get_texture(), blend2sdl(request.blend));

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if ((request.flip & HORIZONTAL_FLIP) != 0)
    {
      flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
    }

    if ((request.flip & VERTICAL_FLIP) != 0)
    {
      flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
    }

    RenderCopyEx(m_sdl_renderer, texture.get_texture(),
                 &src_rect, &dst_rect,
                 static_cast<double>(request.angles[i]), nullptr, flip,
                 texture.get_sampler());
  }
}

void
SDLPainter::draw_gradient(const GradientRequest& request)
{
  const Color& top = request.top;
  const Color& bottom = request.bottom;
  const GradientDirection& direction = request.direction;
  const Rectf& region = request.region;

  // calculate the maximum number of steps needed for the gradient
  int n = static_cast<int>(std::max(std::max(fabsf(top.red - bottom.red),
                                             fabsf(top.green - bottom.green)),
                                    std::max(fabsf(top.blue - bottom.blue),
                                             fabsf(top.alpha - bottom.alpha))) * 255);
  n = std::max(n, 1);

  int next_step = (direction == VERTICAL || direction == VERTICAL_SECTOR) ?
                  static_cast<int>(region.get_top()) : static_cast<int>(region.get_left());
  for (int i = 0; i < n; ++i)
  {
    SDL_Rect rect;

    if (direction == VERTICAL || direction == VERTICAL_SECTOR)
    {
      rect.x = static_cast<int>(region.get_left());
      rect.y = next_step;
      rect.w = static_cast<int>(region.get_right() - region.get_left());
      rect.h = static_cast<int>(ceilf((region.get_bottom() - region.get_top()) / static_cast<float>(n)));

      // Account for the build-up of rounding errors due to floating point precision.
      if (next_step > static_cast<int>(region.get_top() + (region.get_bottom() - region.get_top()) * static_cast<float>(i) / static_cast<float>(n)))
        --rect.h;

      next_step += rect.h;
    }
    else
    {
      rect.x = next_step;
      rect.y = static_cast<int>(region.get_top());
      rect.w = static_cast<int>(ceilf((region.get_right() - region.get_left()) / static_cast<float>(n)));
      rect.h = static_cast<int>(region.get_bottom() - region.get_top());

      // Account for the build-up of rounding errors due to floating point precision.
      if (next_step > static_cast<int>(region.get_left() + (region.get_right() - region.get_left()) * static_cast<float>(i) / static_cast<float>(n)))
        --rect.w;

      next_step += rect.w;
    }

    float p = static_cast<float>(i) / static_cast<float>(n == 1 ? n : n - 1);
    Uint8 r, g, b, a;

    if (direction == HORIZONTAL_SECTOR || direction == VERTICAL_SECTOR)
    {
      float begin_percentage, end_percentage;
      if (direction == HORIZONTAL_SECTOR)
      {
        begin_percentage = -region.get_left() / region.get_right();
        end_percentage = (-region.get_left() + static_cast<float>(SCREEN_WIDTH)) / region.get_right();
      }
      else
      {
        begin_percentage = -region.get_top() / region.get_bottom();
        end_percentage = (-region.get_top() + static_cast<float>(SCREEN_HEIGHT)) / region.get_bottom();
      }

      // This is needed because the limited floating point precision can produce
      // values just below zero or just above one.
      begin_percentage = math::clamp(begin_percentage, 0.0f, 1.0f);
      end_percentage   = math::clamp(end_percentage,   0.0f, 1.0f);

      Color begin, end;
      begin.red   = top.red   * (1.0f - begin_percentage) + bottom.red   * begin_percentage;
      begin.green = top.green * (1.0f - begin_percentage) + bottom.green * begin_percentage;
      begin.blue  = top.blue  * (1.0f - begin_percentage) + bottom.blue  * begin_percentage;
      begin.alpha = top.alpha * (1.0f - begin_percentage) + bottom.alpha * begin_percentage;

      end.red   = top.red   * (1.0f - end_percentage) + bottom.red   * end_percentage;
      end.green = top.green * (1.0f - end_percentage) + bottom.green * end_percentage;
      end.blue  = top.blue  * (1.0f - end_percentage) + bottom.blue  * end_percentage;
      end.alpha = top.alpha * (1.0f - end_percentage) + bottom.alpha * end_percentage;

      r = static_cast<Uint8>(((1.0f - p) * begin.red   + p * end.red)   * 255);
      g = static_cast<Uint8>(((1.0f - p) * begin.green + p * end.green) * 255);
      b = static_cast<Uint8>(((1.0f - p) * begin.blue  + p * end.blue)  * 255);
      a = static_cast<Uint8>(((1.0f - p) * begin.alpha + p * end.alpha) * 255);
    }
    else
    {
      r = static_cast<Uint8>(((1.0f - p) * top.red   + p * bottom.red)   * 255);
      g = static_cast<Uint8>(((1.0f - p) * top.green + p * bottom.green) * 255);
      b = static_cast<Uint8>(((1.0f - p) * top.blue  + p * bottom.blue)  * 255);
      a = static_cast<Uint8>(((1.0f - p) * top.alpha + p * bottom.alpha) * 255);
    }

    SDL_SetRenderDrawBlendMode(m_sdl_renderer, blend2sdl(request.blend));
    SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
    SDL_RenderFillRect(m_sdl_renderer, &rect);
  }
}

void
SDLPainter::draw_filled_rect(const FillRectRequest& request)
{
  SDL_FRect rect = request.rect.to_sdl();

  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  const float radius = std::min(std::min(rect.h / 2, rect.w / 2), request.radius);

  if (radius > 0.f)
  {
    const int slices = static_cast<int>(radius);

    // rounded top and bottom parts
    std::vector<SDL_FRect> rects;
    rects.reserve(2*slices + 1);
    for (int i = 0; i < slices; ++i)
    {
      float p = (static_cast<float>(i) + 0.5f) / radius;
      float xoff = radius - sqrtf(1.0f - p * p) * radius;

      SDL_FRect tmp;
      tmp.x = rect.x + xoff;
      tmp.y = rect.y + (radius - static_cast<float>(i));
      tmp.w = rect.w - 2*(xoff);
      tmp.h = 1;
      rects.push_back(tmp);

      SDL_FRect tmp2;
      tmp2.x = rect.x + xoff;
      tmp2.y = rect.y + rect.h - radius + static_cast<float>(i);
      tmp2.w = rect.w - 2*xoff;
      tmp2.h = 1;

      if (tmp2.y != tmp.y)
      {
        rects.push_back(tmp2);
      }
    }

    if (2*radius < rect.h)
    {
      // center rectangle
      SDL_FRect tmp;
      tmp.x = rect.x;
      tmp.y = rect.y + radius + 1;
      tmp.w = rect.w;
      tmp.h = rect.h - 2*radius - 1;
      rects.push_back(tmp);
    }

    SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
    SDL_RenderFillRectsF(m_sdl_renderer, &*rects.begin(), static_cast<int>(rects.size()));
  }
  else
  {
    if ((rect.w != 0) && (rect.h != 0))
    {
      SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
      SDL_RenderFillRectF(m_sdl_renderer, &rect);
    }
  }
}

void
SDLPainter::draw_inverse_ellipse(const InverseEllipseRequest& request)
{
  float x = request.pos.x;
  float w = request.size.x;
  float h = request.size.y;

  float top = request.pos.y - (h / 2);

  const Viewport& viewport = m_video_system.get_viewport();

  const int max_slices = 256;
  SDL_FRect rects[2*max_slices+2];
  int slices = std::min(static_cast<int>(request.size.y), max_slices);
  for (int i = 0; i < slices; ++i)
  {
    float p = ((static_cast<float>(i) + 0.5f) / static_cast<float>(slices)) * 2.0f - 1.0f;
    float xoff = sqrtf(1.0f - p*p) * w / 2;

    SDL_FRect& left  = rects[2*i+0];
    SDL_FRect& right = rects[2*i+1];

    left.x = 0;
    left.y = top + (static_cast<float>(i) * h / static_cast<float>(slices));
    left.w = x - xoff;
    left.h = top + ((static_cast<float>(i + 1) * h / static_cast<float>(slices)) - left.y);

    right.x = x + xoff;
    right.y = left.y;
    right.w = static_cast<float>(viewport.get_screen_width()) - right.x;
    right.h = left.h;
  }

  SDL_FRect& top_rect = rects[2*slices+0];
  SDL_FRect& bottom_rect = rects[2*slices+1];

  top_rect.x = 0;
  top_rect.y = 0;
  top_rect.w = static_cast<float>(viewport.get_screen_width());
  top_rect.h = top;

  bottom_rect.x = 0;
  bottom_rect.y = top + h;
  bottom_rect.w = static_cast<float>(viewport.get_screen_width());
  bottom_rect.h = static_cast<float>(viewport.get_screen_height()) - bottom_rect.y;

  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
  SDL_RenderFillRectsF(m_sdl_renderer, rects, 2*slices+2);
}

void
SDLPainter::draw_line(const LineRequest& request)
{
  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
  SDL_RenderDrawLineF(m_sdl_renderer, request.pos.x, request.pos.y,
                                      request.dest_pos.x, request.dest_pos.y);
}

namespace {

using Edge = std::pair<const Vector&, const Vector&>;

Edge
make_edge(const Vector& p1, const Vector& p2)
{
  if (p1.y < p2.y)
    return Edge(p1, p2);

  return Edge(p2, p1);
}

void
draw_span_between_edges(SDL_Renderer* renderer, const Edge& e1, const Edge& e2)
{
  // calculate difference between the y coordinates
  // of the first edge and return if 0
  float e1ydiff = e1.second.y - e1.first.y;
  if (e1ydiff == 0.0f)
    return;

  // calculate difference between the y coordinates
  // of the second edge and return if 0
  float e2ydiff = e2.second.y - e2.first.y;
  if (e2ydiff == 0.0f)
    return;

  float e1xdiff = e1.second.x - e1.first.x;
  float e2xdiff = e2.second.x - e2.first.x;
  float factor1 = (e2.first.y - e1.first.y) / e1ydiff;
  float factorStep1 = 1.0f / e1ydiff;
  float factor2 = 0.0f;
  float factorStep2 = 1.0f / e2ydiff;

  for (float y = e2.first.y; y < e2.second.y; y += 1.f)
  {
    SDL_RenderDrawLineF(renderer,
                        e1.first.x + e1xdiff * factor1, y,
                        e2.first.x + e2xdiff * factor2, y);
    factor1 += factorStep1;
    factor2 += factorStep2;
  }
}

} // namespace

void
SDLPainter::draw_triangle(const TriangleRequest& request)
{
  Uint8 r = static_cast<Uint8>(request.color.red * 255);
  Uint8 g = static_cast<Uint8>(request.color.green * 255);
  Uint8 b = static_cast<Uint8>(request.color.blue * 255);
  Uint8 a = static_cast<Uint8>(request.color.alpha * 255);

  std::array<Edge, 3> edges{
    make_edge(request.pos1, request.pos2),
    make_edge(request.pos2, request.pos3),
    make_edge(request.pos3, request.pos1)
  };

  float maxLength = 0.f;
  int longEdge = 0;

  // Find the edge with the greatest length on the Y axis.
  for (int i = 0; i < 3; i++)
  {
    const float length = edges[i].second.y - edges[i].first.y;
    if (length > maxLength)
    {
      maxLength = length;
      longEdge = i;
    }
  }
  int shortEdge1 = (longEdge + 1) % 3;
  int shortEdge2 = (longEdge + 2) % 3;

  SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);

  draw_span_between_edges(m_sdl_renderer, edges[longEdge], edges[shortEdge1]);
  draw_span_between_edges(m_sdl_renderer, edges[longEdge], edges[shortEdge2]);
}

void
SDLPainter::clear(const Color& color)
{
  SDL_SetRenderDrawColor(m_sdl_renderer, color.r8(), color.g8(), color.b8(), color.a8());

  if (m_cliprect)
  {
    SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_NONE);
    SDL_RenderFillRect(m_sdl_renderer, &*m_cliprect);
  }
  else
  {
    // This ignores the cliprect:
    SDL_RenderClear(m_sdl_renderer);
  }
}

void
SDLPainter::set_clip_rect(const Rect& rect)
{
  m_cliprect = rect.to_sdl();

  int ret = SDL_RenderSetClipRect(m_sdl_renderer, &*m_cliprect);
  if (ret < 0)
  {
    log_warning << "SDLPainter::set_clip_rect(): SDL_RenderSetClipRect() failed: " << SDL_GetError() << std::endl;
  }
}

void
SDLPainter::clear_clip_rect()
{
  m_cliprect.reset();

  int ret = SDL_RenderSetClipRect(m_sdl_renderer, nullptr);
  if (ret < 0)
  {
    log_warning << "SDLPainter::clear_clip_rect(): SDL_RenderSetClipRect() failed: " << SDL_GetError() << std::endl;
  }
}

void
SDLPainter::get_pixel(const GetPixelRequest& request) const
{
  const Rect& rect = m_renderer.get_rect();
  const Size& logical_size = m_renderer.get_logical_size();

  SDL_Rect srcrect;
  srcrect.x = rect.left + static_cast<int>(request.pos.x * static_cast<float>(rect.get_width()) / static_cast<float>(logical_size.width));
  srcrect.y = rect.top + static_cast<int>(request.pos.y * static_cast<float>(rect.get_height()) / static_cast<float>(logical_size.height));
  srcrect.w = 1;
  srcrect.h = 1;

  Uint8 pixel[4];
  int ret = SDL_RenderReadPixels(m_sdl_renderer, &srcrect,
                                 SDL_PIXELFORMAT_RGB888,
                                 pixel,
                                 1);
  if (ret != 0)
  {
    log_warning << "failed to read pixels: " << SDL_GetError() << std::endl;
  }

  *(request.color_ptr) = Color::from_rgb888(pixel[2], pixel[1], pixel[0]);
}

/* EOF */
