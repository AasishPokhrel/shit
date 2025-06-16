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

#include "video/sampler.hpp"

Sampler::Sampler() :
  m_filter(GL_LINEAR),
  m_wrap_s(GL_CLAMP_TO_EDGE),
  m_wrap_t(GL_CLAMP_TO_EDGE),
  m_animate(0.0f, 0.0f)
{
}

Sampler::Sampler(GLenum filter, GLenum wrap_s, GLenum wrap_t, const Vector& animate) :
  m_filter(filter),
  m_wrap_s(wrap_s),
  m_wrap_t(wrap_t),
  m_animate(animate)
{
}

/* EOF */
