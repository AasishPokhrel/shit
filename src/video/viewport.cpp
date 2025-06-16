//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#include "viewport.hpp"

#include <algorithm>

#include "config.h"

#include "math/rect.hpp"
#include "math/size.hpp"
#include "math/vector.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"

// Minimum and maximum size of the virtual screen, note that the
// maximum must not exceed X/Y_OFFSCREEN_DISTANCE or enemies end up
// spawning on screen instead of off-screen.
const Size Viewport::s_max_size(1368, 800);
const Size Viewport::s_min_size(640, 480);

namespace {

inline Size
apply_pixel_aspect_ratio_pre(const Size& window_size, float pixel_aspect_ratio)
{
  return Size(static_cast<int>(static_cast<float>(window_size.width) * pixel_aspect_ratio),
              window_size.height);
}

inline void
apply_pixel_aspect_ratio_post(const Size& real_window_size, const Size& window_size, float scale,
                              Rect& out_viewport, Vector& out_scale)
{
  Vector transform(static_cast<float>(real_window_size.width) / static_cast<float>(window_size.width),
                   static_cast<float>(real_window_size.height) / static_cast<float>(window_size.height));

  out_viewport.left = static_cast<int>(static_cast<float>(out_viewport.left) * transform.x);
  out_viewport.top = static_cast<int>(static_cast<float>(out_viewport.top) * transform.y);
  out_viewport.right = static_cast<int>(static_cast<float>(out_viewport.right) * transform.x);
  out_viewport.bottom = static_cast<int>(static_cast<float>(out_viewport.bottom) * transform.y);

  out_scale.x = scale * transform.x;
  out_scale.y = scale * transform.y;
}

inline float
calculate_scale(const Size& min_size, const Size& max_size,
                const Size& window_size,
                float magnification)
{
  float scale = magnification;
  if (scale == 0.0f) // magic value
  {
    scale = 1.0f;

    // Find the minimum magnification that is needed to fill the screen
    if (window_size.width > max_size.width ||
        window_size.height > max_size.height)
    {
      scale = std::max(static_cast<float>(window_size.width) / static_cast<float>(max_size.width),
                       static_cast<float>(window_size.height) / static_cast<float>(max_size.height));
    }

    // If the resulting area would violate min_size, scale it down
    if (static_cast<float>(window_size.width) / scale < static_cast<float>(min_size.width) ||
        static_cast<float>(window_size.height) / scale < static_cast<float>(min_size.height))
    {
      scale = std::min(static_cast<float>(window_size.width) / static_cast<float>(min_size.width),
                       static_cast<float>(window_size.height) / static_cast<float>(min_size.height));
    }
  }

  if (g_config->mobile_controls)
    scale = std::max(scale, 1.f);

  return scale;
}

inline Rect
calculate_viewport(const Size& max_size, const Size& window_size, float scale)
{
  int viewport_width = std::min(window_size.width,
                                static_cast<int>(scale * static_cast<float>(max_size.width)));
  int viewport_height = std::min(window_size.height,
                                 static_cast<int>(scale * static_cast<float>(max_size.height)));

  // Center the viewport in the window
  Rect viewport;

  viewport.left = std::max(0, (window_size.width - viewport_width) / 2);
  viewport.top = std::max(0, (window_size.height - viewport_height) / 2);

  viewport.right = viewport.left + viewport_width;
  viewport.bottom = viewport.top + viewport_height;

  return viewport;
}

void calculate_viewport(const Size& min_size, const Size& max_size,
                        const Size& real_window_size,
                        float pixel_aspect_ratio, float magnification,
                        Vector& out_scale,
                        Rect& out_viewport)
{
  // Transform the real window_size by the aspect ratio, then do
  // calculations on that virtual window_size
  Size window_size = apply_pixel_aspect_ratio_pre(real_window_size, pixel_aspect_ratio);

  float scale = calculate_scale(min_size, max_size, window_size, magnification);

  // Calculate the new viewport size
  out_viewport = calculate_viewport(max_size, window_size, scale);

  // Transform the virtual window_size back into real window coordinates
  apply_pixel_aspect_ratio_post(real_window_size, window_size, scale,
                                out_viewport, out_scale);
}

float calculate_pixel_aspect_ratio(const Size& source, const Size& target)
{
  float source_aspect = 16.0f / 9.0f; // random guess
  if (source != Size(0, 0))
  {
    source_aspect =
      static_cast<float>(source.width) /
      static_cast<float>(source.height);
  }

  float target_aspect =
    static_cast<float>(target.width) /
    static_cast<float>(target.height);

  return target_aspect / source_aspect;
}

} // namespace

Viewport
Viewport::from_size(const Size& target_size, const Size& desktop_size)
{
  float pixel_aspect_ratio = 1.0f;
  if (g_config->aspect_size != Size(0, 0))
  {
    pixel_aspect_ratio = calculate_pixel_aspect_ratio(desktop_size,
                                                      g_config->aspect_size);
  }
  else if (g_config->use_fullscreen)
  {
    pixel_aspect_ratio = calculate_pixel_aspect_ratio(desktop_size,
                                                      target_size);
  }

  // calculate the viewport
  Rect viewport;
  Vector scale(0.0f, 0.0f);
  calculate_viewport(s_min_size, s_max_size,
                     target_size,
                     pixel_aspect_ratio,
                     g_config->magnification,
                     scale, viewport);

  return Viewport(viewport, scale);
}

Viewport::Viewport() :
  m_rect(),
  m_scale(0.0f, 0.0f)
{
}

Viewport::Viewport(const Rect& rect, const Vector& scale) :
  m_rect(rect),
  m_scale(scale)
{
}

int
Viewport::get_screen_width() const
{
  return static_cast<int>(static_cast<float>(m_rect.get_width()) / m_scale.x);
}

int
Viewport::get_screen_height() const
{
  return static_cast<int>(static_cast<float>(m_rect.get_height()) / m_scale.y);
}

Size
Viewport::get_screen_size() const
{
  return Size(get_screen_width(), get_screen_height());
}

Vector
Viewport::to_logical(int physical_x, int physical_y) const
{
  return Vector(static_cast<float>(physical_x - m_rect.left) / m_scale.x,
                static_cast<float>(physical_y - m_rect.top) / m_scale.y);
}

bool
Viewport::needs_clear_screen() const
{
  return (m_rect.left != 0 || m_rect.top != 0);
}

/* EOF */
