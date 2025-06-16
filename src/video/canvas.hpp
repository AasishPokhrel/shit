//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_CANVAS_HPP
#define HEADER_SUPERTUX_VIDEO_CANVAS_HPP

#include <string>
#include <vector>
#include <memory>
#include <obstack.h>

#include "math/rectf.hpp"
#include "math/vector.hpp"
#include "video/blend.hpp"
#include "video/color.hpp"
#include "video/drawing_target.hpp"
#include "video/font.hpp"
#include "video/font_ptr.hpp"
#include "video/gl.hpp"
#include "video/gradient.hpp"
#include "video/layer.hpp"
#include "video/paint_style.hpp"

class DrawingContext;
class Renderer;
class VideoSystem;
struct DrawingRequest;

class Canvas final
{
public:
  enum Filter { BELOW_LIGHTMAP, ABOVE_LIGHTMAP, ALL };

public:
  Canvas(DrawingContext& context, obstack& obst);
  ~Canvas();

  void draw_surface(const SurfacePtr& surface, const Vector& position, int layer);
  void draw_surface(const SurfacePtr& surface, const Vector& position, float angle, const Color& color, const Blend& blend,
                    int layer);
  void draw_surface_part(const SurfacePtr& surface, const Rectf& srcrect, const Rectf& dstrect,
                         int layer, const PaintStyle& style = PaintStyle());
  void draw_surface_scaled(const SurfacePtr& surface, const Rectf& dstrect,
                           int layer, const PaintStyle& style = PaintStyle());
  void draw_surface_batch(const SurfacePtr& surface,
                          std::vector<Rectf> srcrects,
                          std::vector<Rectf> dstrects,
                          const Color& color,
                          int layer);
  void draw_surface_batch(const SurfacePtr& surface,
                          std::vector<Rectf> srcrects,
                          std::vector<Rectf> dstrects,
                          std::vector<float> angles,
                          const Color& color,
                          int layer);
  Rectf draw_text(const FontPtr& font, const std::string& text,
                  const Vector& position, FontAlignment alignment, int layer, const Color& color = Color(1.0,1.0,1.0));
  /** Draw text to the center of the screen */
  Rectf draw_center_text(const FontPtr& font, const std::string& text,
                         const Vector& position, int layer, const Color& color = Color(1.0,1.0,1.0));
  void draw_gradient(const Color& from, const Color& to, int layer, const GradientDirection& direction,
                     const Rectf& region, const Blend& blend = Blend());
  void draw_filled_rect(const Rectf& rect, const Color& color, int layer);
  void draw_filled_rect(const Rectf& rect, const Color& color, float radius, int layer);

  void draw_inverse_ellipse(const Vector& pos, const Vector& size, const Color& color, int layer);

  void draw_line(const Vector& pos1, const Vector& pos2, const Color& color, int layer);
  void draw_triangle(const Vector& pos1, const Vector& pos2, const Vector& pos3, const Color& color, int layer);

  /** Draw a flat-topped regular hexagon
   *
   * @param pos Centre position
   * @param radius Radius of the hexagon's circle hull
   * @param color Color
   * @param layer Layer
   */
  void draw_hexagon(const Vector& pos, float radius, const Color& color,
    int layer);

  /** on next update, set color to lightmap's color at position */
  void get_pixel(const Vector& position, const std::shared_ptr<Color>& color_out);

  void clear();
  void render(Renderer& renderer, Filter filter);

  inline DrawingContext& get_context() { return m_context; }

private:
  Vector apply_translate(const Vector& pos) const;
  float scale() const;

private:
  DrawingContext& m_context;
  obstack& m_obst;
  std::vector<DrawingRequest*> m_requests;

private:
  Canvas(const Canvas&) = delete;
  Canvas& operator=(const Canvas&) = delete;
};

#endif

/* EOF */
