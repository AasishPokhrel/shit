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

#include "video/surface_batch.hpp"

#include "math/rectf.hpp"
#include "video/surface.hpp"

SurfaceBatch::SurfaceBatch(const SurfacePtr& surface, const Color& color) :
  m_surface(surface),
  m_color(color),
  m_srcrects(),
  m_dstrects(),
  m_angles()
{
}

void
SurfaceBatch::draw(const Vector& pos, float angle)
{
  m_srcrects.emplace_back(Rectf(0, 0,
                                static_cast<float>(m_surface->get_width()),
                                static_cast<float>(m_surface->get_height())));
  m_dstrects.emplace_back(Rectf(pos,
                                Sizef(static_cast<float>(m_surface->get_width()),
                                      static_cast<float>(m_surface->get_height()))));
  m_angles.emplace_back(angle);
}

void
SurfaceBatch::draw(const Rectf& dstrect, float angle)
{
  m_srcrects.emplace_back(Rectf(0, 0,
                                static_cast<float>(m_surface->get_width()),
                                static_cast<float>(m_surface->get_height())));
  m_dstrects.emplace_back(dstrect);
  m_angles.emplace_back(angle);
}

void
SurfaceBatch::draw(const Rectf& srcrect, const Rectf& dstrect, float angle)
{
  m_srcrects.emplace_back(srcrect);
  m_dstrects.emplace_back(dstrect);
  m_angles.emplace_back(angle);
}

/* EOF */
