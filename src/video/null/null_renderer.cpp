//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/null/null_renderer.hpp"

#include "util/log.hpp"
#include "video/null/null_painter.hpp"

NullRenderer::NullRenderer() :
  m_painter(new NullPainter)
{
}

NullRenderer::~NullRenderer()
{
}

void
NullRenderer::start_draw()
{
  log_info << "NullRenderer::start_draw()" << std::endl;
}

void
NullRenderer::end_draw()
{
  log_info << "NullRenderer::end_draw()" << std::endl;
}

Painter&
NullRenderer::get_painter()
{
  return *m_painter;
}

Rect
NullRenderer::get_rect() const
{
  return Rect();
}

Size
NullRenderer::get_logical_size() const
{
  return Size();
}

TexturePtr
NullRenderer::get_texture() const
{
  return {};
}

/* EOF */
