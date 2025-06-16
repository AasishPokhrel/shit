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

#include "video/drawing_context.hpp"

#include <algorithm>

#include "supertux/globals.hpp"
#include "util/obstackpp.hpp"
#include "video/drawing_request.hpp"
#include "video/renderer.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

DrawingContext::DrawingContext(VideoSystem& video_system_, obstack& obst, bool overlay, float time_offset) :
  m_video_system(video_system_),
  m_obst(obst),
  m_overlay(overlay),
  m_ambient_color(Color::WHITE),
  m_transform_stack({ DrawingTransform(m_video_system.get_viewport()) }),
  m_colormap_canvas(*this, m_obst),
  m_lightmap_canvas(*this, m_obst),
  m_time_offset(time_offset)
{
}

DrawingContext::~DrawingContext()
{
  clear();
}

void
DrawingContext::clear()
{
  m_lightmap_canvas.clear();
  m_colormap_canvas.clear();
}

Rectf
DrawingContext::get_cliprect() const
{
  return Rectf(get_translation().x,
               get_translation().y,
               get_translation().x + static_cast<float>(transform().viewport.get_width()) / transform().scale,
               get_translation().y + static_cast<float>(transform().viewport.get_height()) / transform().scale);
}

Canvas&
DrawingContext::get_canvas(DrawingTarget target)
{
  switch (target)
  {
    case DrawingTarget::LIGHTMAP:
      return light();

    default:
      return color();
  }
}

float
DrawingContext::get_width() const
{
  return static_cast<float>(transform().viewport.get_width()) / transform().scale;
}

float
DrawingContext::get_height() const
{
  return static_cast<float>(transform().viewport.get_height()) / transform().scale;
}

Vector
DrawingContext::get_size() const
{
  return Vector(get_width(), get_height()) * transform().scale;
}

bool
DrawingContext::use_lightmap() const
{
  return !m_overlay && m_ambient_color != Color::WHITE;
}

bool
DrawingContext::perspective_scale(float speed_x, float speed_y)
{
  DrawingTransform& tfm = transform();
  if (tfm.scale == 1 || speed_x < 0 || speed_y < 0) {
    //Trivial or unreal situation: Do not apply perspective.
    return true;
  }
  const float speed = sqrt(speed_x * speed_y);
  if (speed == 0) {
    //Special case: The object appears to be infinitely far.
    tfm.scale = 1.0;
    return true;
  }
  const float t = tfm.scale * (1 / speed - 1) + 1;
  if (t <= 0) {
    //The object will appear behind the camera, therefore we shall not see it.
    return false;
  }
  tfm.scale /= speed * t;
  return true;
}

/* EOF */
