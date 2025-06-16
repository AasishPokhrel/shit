//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#include "editor/marker_object.hpp"

#include "supertux/resources.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

MarkerObject::MarkerObject (const Vector& pos)
{
  m_col.m_bbox.set_p1(pos);
  m_col.m_bbox.set_size(16, 16);
}

MarkerObject::MarkerObject ()
{
  m_col.m_bbox.set_p1(Vector(0, 0));
  m_col.m_bbox.set_p2(Vector(16, 16));
}

void
MarkerObject::draw(DrawingContext& context)
{
  Vector dir = get_point_vector();
  if (dir.x == 0 && dir.y == 0) {
    if (hide_if_no_offset())
      return;
    context.color().draw_filled_rect(m_col.m_bbox, Color(1, 1, 1, 0.5), 7.5, LAYER_GUI-20);
  } else {
    // draw a triangle
    dir = 8.0f * glm::normalize(dir);
    Vector dir2 = Vector(-dir.y, dir.x);
    Vector pos = m_col.m_bbox.get_middle();
    context.color().draw_triangle(pos + dir * 1.5f, pos - dir + dir2, pos - dir - dir2,
                                    Color(1, 1, 1, 0.5f), LAYER_GUI-20);
  }
}

/* EOF */
