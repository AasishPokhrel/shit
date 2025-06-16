//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "editor/object_icon.hpp"

#include "math/rectf.hpp"
#include "util/reader_mapping.hpp"
#include "video/surface.hpp"
#include "video/drawing_context.hpp"

ObjectIcon::ObjectIcon(const std::string& object_class, const std::string& icon) :
  m_object_class(object_class),
  m_surface(Surface::from_file(icon)),
  m_offset(0.0f, 0.0f)
{
  calculate_offset();
}

ObjectIcon::ObjectIcon(const ReaderMapping& reader) :
  m_object_class(),
  m_surface(),
  m_offset(0.0f, 0.0f)
{
  std::string icon = "images/engine/icons/supertux.png";
  reader.get("class", m_object_class);
  reader.get("icon", icon);
  m_surface = Surface::from_file(icon);
  calculate_offset();
}

ObjectIcon::~ObjectIcon()
{

}

void
ObjectIcon::calculate_offset()
{
  float w = static_cast<float>(m_surface->get_width());
  float h = static_cast<float>(m_surface->get_height());

  if (w > h) {
    m_offset.x = 0;
    m_offset.y = 32/w * (w - h) / 2;
  } else {
    m_offset.y = 0;
    m_offset.x = 32/h * (h - w) / 2;
  }
}

void
ObjectIcon::draw(DrawingContext& context, const Vector& pos)
{
  context.color().draw_surface_scaled(m_surface,
                                      Rectf(
                                        pos + m_offset,
                                        pos + Vector(32, 32) - m_offset
                                      ),
                                      LAYER_GUI - 9);
}

void
ObjectIcon::draw(DrawingContext& context, const Vector& pos, int pixels_shown)
{
  auto cropped_surface = m_surface->region(Rect(32 - pixels_shown,
                                                0,
                                                32,
                                                32));
  context.color().draw_surface_scaled(cropped_surface,
                                      Rectf(
                                        pos + Vector(static_cast<float>(32 - pixels_shown), 0) + m_offset,
                                        pos + Vector(32, 32) - m_offset
                                      ),
                                      LAYER_GUI - 9);
}

/* EOF */
