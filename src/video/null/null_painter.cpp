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

#include "video/null/null_painter.hpp"

#include "util/log.hpp"

NullPainter::NullPainter() :
  m_clip_rect()
{
}

NullPainter::~NullPainter()
{
}

void
NullPainter::draw_texture(const TextureRequest& request)
{
  log_info << "NullPainter::draw_texture()" << std::endl;
}

void
NullPainter::draw_gradient(const GradientRequest& request)
{
  log_info << "NullPainter::draw_gradient()" << std::endl;
}

void
NullPainter::draw_filled_rect(const FillRectRequest& request)
{
  log_info << "NullPainter::draw_filled_rect()" << std::endl;
}

void
NullPainter::draw_inverse_ellipse(const InverseEllipseRequest& request)
{
  log_info << "NullPainter::draw_inverse_ellipse()" << std::endl;
}

void
NullPainter::draw_line(const LineRequest& request)
{
  log_info << "NullPainter::draw_line()" << std::endl;
}

void
NullPainter::draw_triangle(const TriangleRequest& request)
{
  log_info << "NullPainter::draw_triangle()" << std::endl;
}


void
NullPainter::clear(const Color& color)
{
  log_info << "NullPainter::clear()" << std::endl;
}

void
NullPainter::get_pixel(const GetPixelRequest& request) const
{
  log_info << "NullPainter::get_pixel()" << std::endl;
}

void
NullPainter::set_clip_rect(const Rect& rect)
{
  log_info << "NullPainter::set_clip_rect()" << std::endl;
  m_clip_rect = rect;
}

void
NullPainter::clear_clip_rect()
{
  log_info << "NullPainter::clear_clip_rect()" << std::endl;
  m_clip_rect = std::nullopt;
}

/* EOF */
