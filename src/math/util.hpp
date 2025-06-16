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

#ifndef HEADER_SUPERTUX_MATH_UTIL_HPP
#define HEADER_SUPERTUX_MATH_UTIL_HPP

#include <math.h>

namespace math {

template<class T>
const T& clamp(const T& val, const T& min, const T& max)
{
  if (val < min)
  {
    return min;
  }
  else if (val > max)
  {
    return max;
  }
  else
  {
    return val;
  }
}

template<class T>
bool in_bounds(const T& val, const T& min, const T& max)
{
  return val >= min && val <= max;
}

template <class T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

constexpr float TAU = 6.28318530717958647693f;
constexpr float PI = 3.14159265358979323846f;
constexpr float PI_2 = 1.57079632679489661923f;
constexpr float PI_4 = 0.78539816339744830962f;

inline float degrees(float rad)
{
  return rad / TAU * 360.0f;
}

inline float radians(float deg)
{
  return deg / 360.0f * TAU;
}

inline int positive_mod(int lhs, int rhs)
{
  return (lhs % rhs + rhs) % rhs;
}

inline float positive_fmodf(float lhs, float rhs)
{
  return fmodf(fmodf(lhs, rhs) + rhs, rhs);
}

} // namespace math

#endif

/* EOF */
