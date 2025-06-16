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

#include "math/rect.hpp"

#include <ostream>

#include "math/rectf.hpp"

Rect::Rect(const Rectf& other) :
  left(static_cast<int>(other.get_left())),
  top(static_cast<int>(other.get_top())),
  right(static_cast<int>(other.get_right())),
  bottom(static_cast<int>(other.get_bottom()))
{
}

Rectf
Rect::to_rectf() const
{
  return { static_cast<float>(left), static_cast<float>(top),
           static_cast<float>(right), static_cast<float>(bottom) };
}

std::ostream& operator<<(std::ostream& out, const Rect& rect)
{
  out << "Rect("
      << rect.left << ", " << rect.top << ", "
      << rect.right << ", " << rect.bottom
      << ")";
  return out;
}

/* EOF */
