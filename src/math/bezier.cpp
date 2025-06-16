//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#include "math/bezier.hpp"
#include "util/log.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

Vector
Bezier::get_point(const Vector& p1, const Vector& p2, const Vector& p3,
                  const Vector& p4, float t)
{
  // SPECIAL CASE:
  // If Beziers aren't used, treat it linearly. That is because Beziers will
  // automatically add some "easing"-like effect when unused, which might be
  // undesired.
  // To reproduce the same effect with bezier handles, make it so:
  //    p2 = 1/3rd way from p1 towards p4
  //    p3 = 1/3rd way from p4 towards p1
  //if (p1 == p2 && p3 == p4)
  //  return p1 + (p4 - p1) * t;

  return get_point_raw(p1, p2, p3, p4, t);
}

Vector
Bezier::get_point_raw(const Vector& p1, const Vector& p2, const Vector& p3,
                      const Vector& p4, float t)
{
  float t2 = 1.f - t;

  return p1 * (t2 * t2 * t2) + p2 * t2 * t2 * t * 3.f  + p3 * t2 * t * t * 3.f + p4 * (t * t * t);
}

float
Bezier::get_length(const Vector& p1, const Vector& p2, const Vector& p3,
                   const Vector& p4, int steps)
{
  float fteps = static_cast<float>(steps);
  float length = 0.f;

  for (int i = 1; i <= steps; i++)
  {
    length += glm::length(
                get_point(p1, p2, p3, p4, static_cast<float>(i) / fteps) -
                get_point(p1, p2, p3, p4, static_cast<float>(i - 1) / fteps)
              );
  }

  return length;
}

Vector
Bezier::get_point_at_length(const Vector& p1, const Vector& p2, const Vector& p3,
                            const Vector& p4, float length, int steps)
{
  if (length == 0)
    return p1;

  float fteps = static_cast<float>(steps);

  for (int i = 1; i <= steps; i++)
  {
    Vector lastpos = get_point(p1, p2, p3, p4, static_cast<float>(i) / fteps);
    Vector pos     = get_point(p1, p2, p3, p4, static_cast<float>(i - 1) / fteps);
    float step = glm::length(lastpos - pos);
    length -= step;

    if (length <= 0)
      return lastpos + (pos - lastpos) * (-length / step);
  }

  // The length might be equal to something like 4.86e-05 if the original length
  // was equal or close to the total length, due to float's limited precision.
  // We'll consider it a problem if the difference is greater than 0.001.
  if (length > 0.001f)
    log_warning << "Attempt to get point on Bezier curve further than the end: " << length << std::endl;

  return p4;
}

Vector
Bezier::get_point_by_length(const Vector& p1, const Vector& p2, const Vector& p3,
                  const Vector& p4, float t)
{
  return get_point_at_length(p1, p2, p3, p4, get_length(p1, p2, p3, p4) * t);
}

void
Bezier::draw_curve(DrawingContext& context, const Vector& p1, const Vector& p2,
                   const Vector& p3, const Vector& p4, int steps, Color color,
                   int layer)
{
  // Save ourselves some processing time in common special cases.
  if (p1 == p2 && p3 == p4)
  {
    context.color().draw_line(p1, p4, color, layer);
    return;
  }

  for (int i = 0; i < steps; i += 1)
  {
    float f1 = static_cast<float>(i) / static_cast<float>(steps),
          f2 = static_cast<float>(i + 1) / static_cast<float>(steps);

    context.color().draw_line(get_point(p1, p2, p3, p4, f1),
                              get_point(p1, p2, p3, p4, f2),
                              color,
                              layer);
  }
}

/* EOF */
