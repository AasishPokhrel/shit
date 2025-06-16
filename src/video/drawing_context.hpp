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

#ifndef HEADER_SUPERTUX_VIDEO_DRAWING_CONTEXT_HPP
#define HEADER_SUPERTUX_VIDEO_DRAWING_CONTEXT_HPP

#include <string>
#include <vector>
#include <obstack.h>
#include <optional>

#include "math/rect.hpp"
#include "math/rectf.hpp"
#include "math/vector.hpp"
#include "video/canvas.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/drawing_transform.hpp"
#include "video/font.hpp"
#include "video/font_ptr.hpp"

class VideoSystem;
struct DrawingRequest;
struct obstack;

/** This class provides functions for drawing things on screen. It
    also maintains a stack of transforms that are applied to
    graphics. */
class DrawingContext final
{
public:
  DrawingContext(VideoSystem& video_system, obstack& obst, bool overlay, float time_offset);
  ~DrawingContext();

  /** Returns the visible area in world coordinates */
  Rectf get_cliprect() const;

  inline Canvas& color() { return m_colormap_canvas; }
  inline Canvas& light() { assert(!m_overlay); return m_lightmap_canvas; }
  Canvas& get_canvas(DrawingTarget target);

  inline void set_ambient_color(Color ambient_color) { m_ambient_color = ambient_color; }
  inline Color get_ambient_color() const { return m_ambient_color; }

  inline void push_transform()
  {
    m_transform_stack.push_back(transform());
  }
  inline void pop_transform()
  {
    m_transform_stack.pop_back();
    assert(!m_transform_stack.empty());
  }
  inline DrawingTransform& transform()
  {
    assert(!m_transform_stack.empty());
    return m_transform_stack.back();
  }
  inline const DrawingTransform& transform() const
  {
    assert(!m_transform_stack.empty());
    return m_transform_stack.back();
  }

  inline const Vector& get_translation() const { return transform().translation; }
  inline void set_translation(const Vector& translation) { transform().translation = translation; }

  inline float get_scale() const { return transform().scale; }
  inline void scale(float scale) { transform().scale *= scale; }
  
  /** Recalculates the scaling factor for parallax layers.*/
  bool perspective_scale(float speed_x, float speed_y);

  /** Apply that flip in the next draws (flips are listed on surface.h). */
  inline void set_flip(Flip flip) { transform().flip = flip; }
  inline Flip get_flip() const { return transform().flip; }

  /** apply that alpha in the next draws (1.0 means fully opaque) */
  inline void set_alpha(float alpha) { transform().alpha = alpha; } 
  inline float get_alpha() const { return transform().alpha; }

  /** For position extrapolation at high frame rates: real time since last game update step */
  inline void set_time_offset(float time_offset) { m_time_offset = time_offset; }
  inline float get_time_offset() const { return m_time_offset; }

  void clear();

  inline void set_viewport(const Rect& viewport) { transform().viewport = viewport; }
  inline const Rect& get_viewport() const { return transform().viewport; }

  float get_width() const;
  float get_height() const;
  Vector get_size() const;
  inline Rectf get_rect() const { return Rectf(Vector(0, 0), get_size()); }

  bool use_lightmap() const;

  inline bool is_overlay() const { return m_overlay; }

private:
  VideoSystem& m_video_system;

  /** obstack holds the memory of all the drawing requests, it is
      shared with the Canvas */
  obstack& m_obst;

  /** A context marked as overlay will not have it's light section
      rendered. */
  bool m_overlay;

  Color m_ambient_color;
  std::vector<DrawingTransform> m_transform_stack;

  Canvas m_colormap_canvas;
  Canvas m_lightmap_canvas;

  float m_time_offset;

private:
  DrawingContext(const DrawingContext&) = delete;
  DrawingContext& operator=(const DrawingContext&) = delete;
};

#endif

/* EOF */
