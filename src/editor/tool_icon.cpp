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

#include "editor/tool_icon.hpp"

#include "video/drawing_context.hpp"
#include "video/surface.hpp"

ToolIcon::ToolIcon(const std::string& icon) :
  m_pos(0, 0),
  m_surfaces(),
  m_mode(0),
  m_surf_count(0)
{
  push_mode(icon);
}

void
ToolIcon::push_mode(const std::string& icon)
{
  auto surface = Surface::from_file(icon);
  m_surfaces.push_back(surface);
  m_surf_count++;
}

void
ToolIcon::draw(DrawingContext& context)
{
  context.color().draw_surface(m_surfaces[m_mode], m_pos, LAYER_GUI - 9);
}

void
ToolIcon::next_mode()
{
  m_mode++;
  if (m_mode >= m_surf_count) {
    m_mode = 0;
  }
}

SurfacePtr
ToolIcon::get_current_surface() const
{
  return m_surfaces[m_mode];
}

/* EOF */
