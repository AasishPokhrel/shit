//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "collision/collision.hpp"

#include <algorithm>

#include "math/aatriangle.hpp"
#include "math/rectf.hpp"

namespace collision {

void Constraints::merge_constraints(const Constraints& other)
{
  constrain_left(other.position_left);
  constrain_right(other.position_right);
  constrain_top(other.position_top);
  constrain_bottom(other.position_bottom);

  hit.left |= other.hit.left;
  hit.right |= other.hit.right;
  hit.top |= other.hit.top;
  hit.bottom |= other.hit.bottom;
  hit.crush |= other.hit.crush;
}

//---------------------------------------------------------------------------

namespace {
inline void makePlane(const Vector& p1, const Vector& p2, Vector& n, float& c)
{
  n = Vector(p2.y - p1.y, p1.x - p2.x);
  c = -glm::dot(p2, n);
  float nval = glm::length(n);
  n /= nval;
  c /= nval;
}

}

bool rectangle_aatriangle(Constraints* constraints, const Rectf& rect,
                          const AATriangle& triangle)
{
  bool dummy;
  return rectangle_aatriangle(constraints, rect, triangle, dummy);
}

bool rectangle_aatriangle(Constraints* constraints, const Rectf& rect,
                          const AATriangle& triangle,
                          bool& hits_rectangle_bottom)
{
  if (!rect.overlaps(triangle.bbox))
    return false;

  Vector normal(0.0f, 0.0f);
  float c = 0.0;
  Vector p1(0.0f, 0.0f);
  Rectf area;
  switch (triangle.dir & AATriangle::DEFORM_MASK) {
    case 0:
      area.set_p1(triangle.bbox.p1());
      area.set_p2(triangle.bbox.p2());
      break;
    case AATriangle::DEFORM_BOTTOM:
      area.set_p1(Vector(triangle.bbox.get_left(), triangle.bbox.get_top() + triangle.bbox.get_height()/2));
      area.set_p2(triangle.bbox.p2());
      break;
    case AATriangle::DEFORM_TOP:
      area.set_p1(triangle.bbox.p1());
      area.set_p2(Vector(triangle.bbox.get_right(), triangle.bbox.get_top() + triangle.bbox.get_height()/2));
      break;
    case AATriangle::DEFORM_LEFT:
      area.set_p1(triangle.bbox.p1());
      area.set_p2(Vector(triangle.bbox.get_left() + triangle.bbox.get_width()/2, triangle.bbox.get_bottom()));
      break;
    case AATriangle::DEFORM_RIGHT:
      area.set_p1(Vector(triangle.bbox.get_left() + triangle.bbox.get_width()/2, triangle.bbox.get_top()));
      area.set_p2(triangle.bbox.p2());
      break;
    default:
      assert(false);
  }

  switch (triangle.dir & AATriangle::DIRECTION_MASK) {
    case AATriangle::SOUTHWEST:
      p1 = Vector(rect.get_left(), rect.get_bottom());
      makePlane(area.p1(), area.p2(), normal, c);
      break;
    case AATriangle::NORTHEAST:
      p1 = Vector(rect.get_right(), rect.get_top());
      makePlane(area.p2(), area.p1(), normal, c);
      break;
    case AATriangle::SOUTHEAST:
      p1 = rect.p2();
      makePlane(Vector(area.get_left(), area.get_bottom()),
                Vector(area.get_right(), area.get_top()), normal, c);
      break;
    case AATriangle::NORTHWEST:
      p1 = rect.p1();
      makePlane(Vector(area.get_right(), area.get_top()),
                Vector(area.get_left(), area.get_bottom()), normal, c);
      break;
    default:
      assert(false);
  }

  float n_p1 = -glm::dot(normal, p1);
  float depth = n_p1 - c;
  if (depth < 0)
    return false;

#if 0
  std::cout << "R: " << rect << " Tri: " << triangle << "\n";
  std::cout << "Norm: " << normal << " Depth: " << depth << "\n";
#endif

  Vector outvec = normal * (depth + 0.2f);

  const float RDELTA = 3;
  if (p1.x < area.get_left() - RDELTA || p1.x > area.get_right() + RDELTA
     || p1.y < area.get_top() - RDELTA || p1.y > area.get_bottom() + RDELTA) {
    set_rectangle_rectangle_constraints(constraints, rect, area);
  } else {
    if (outvec.x < 0) {
      constraints->constrain_right(rect.get_right() + outvec.x);
      constraints->hit.right = true;
    } else {
      constraints->constrain_left(rect.get_left() + outvec.x);
      constraints->hit.left = true;
    }

    if (outvec.y < 0) {
      constraints->constrain_bottom(rect.get_bottom() + outvec.y);
      constraints->hit.bottom = true;
      hits_rectangle_bottom = true;
    } else {
      constraints->constrain_top(rect.get_top() + outvec.y);
      constraints->hit.top = true;
    }
    constraints->hit.slope_normal = normal;
  }

  return true;
}

void set_rectangle_rectangle_constraints(Constraints* constraints, const Rectf& r1, const Rectf& r2)
{
  float itop = r1.get_bottom() - r2.get_top();
  float ibottom = r2.get_bottom() - r1.get_top();
  float ileft = r1.get_right() - r2.get_left();
  float iright = r2.get_right() - r1.get_left();

  float vert_penetration = std::min(itop, ibottom);
  float horiz_penetration = std::min(ileft, iright);
  if (vert_penetration < horiz_penetration) {
    if (itop < ibottom) {
      constraints->constrain_bottom(r2.get_top());
      constraints->hit.bottom = true;
    } else {
      constraints->constrain_top(r2.get_bottom());
      constraints->hit.top = true;
    }
  } else {
    if (ileft < iright) {
      constraints->constrain_right(r2.get_left());
      constraints->hit.right = true;
    } else {
      constraints->constrain_left(r2.get_right());
      constraints->hit.left = true;
    }
  }
}

bool line_intersects_line(const Vector& line1_start, const Vector& line1_end, const Vector& line2_start, const Vector& line2_end)
{
  // Adapted from Striker, (C) 1999 Joris van der Hoeven, GPL

  float a1 = line1_start.x, b1 = line1_start.y, a2 = line1_end.x, b2 = line1_end.y;
  float c1 = line2_start.x, d1 = line2_start.y, c2 = line2_end.x, d2 = line2_end.y;

  float num = (b2-b1)*(c2-c1) - (a2-a1)*(d2-d1);
  float den1 = (d2-b2)*(c1-c2) + (a2-c2)*(d1-d2);
  float den2 = (d2-b2)*(a1-a2) + (a2-c2)*(b1-b2);

  // Normalize to positive numerator.
  if (num < 0) {
    num = -num;
    den1 = -den1;
    den2 = -den2;
  }

  // Numerator is zero -> Check for parallel or coinciding lines.
  if (num == 0) {
    if ((b1-b2)*(c1-a2) != (a1-a2)*(d1-b2)) return false;
    if (a1 == a2) {
      std::swap(a1, b1);
      std::swap(a2, b2);
      std::swap(c1, d1);
      std::swap(c2, d2);
    }
    if (a1 > a2) std::swap(a1, a2);
    if (c1 > c2) std::swap(c1, c2);
    return ((a1 <= c2) && (a2 >= c1));
  }

  // Standard check.
  return (den1>=0) && (den1<=num) && (den2>=0) && (den2<=num);

}

bool intersects_line(const Rectf& r, const Vector& line_start, const Vector& line_end)
{
  Vector p1 = r.p1();
  Vector p2 = Vector(r.get_right(), r.get_top());
  Vector p3 = r.p2();
  Vector p4 = Vector(r.get_left(), r.get_bottom());
  if (line_intersects_line(p1, p2, line_start, line_end)) return true;
  if (line_intersects_line(p2, p3, line_start, line_end)) return true;
  if (line_intersects_line(p3, p4, line_start, line_end)) return true;
  if (line_intersects_line(p4, p1, line_start, line_end)) return true;
  return false;
}

}

/* EOF */
