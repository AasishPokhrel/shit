//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "math/size.hpp"

#include <ostream>

#include "math/sizef.hpp"

Size::Size(const Sizef& rhs) :
  width(static_cast<int>(rhs.width)),
  height(static_cast<int>(rhs.height))
{
}

std::ostream& operator<<(std::ostream& s, const Size& size)
{
  return s << "Size(" << size.width << ", " << size.height << ")";
}

/* EOF */
