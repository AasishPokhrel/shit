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

#ifndef HEADER_SUPERTUX_MATH_RECTF_HPP
#define HEADER_SUPERTUX_MATH_RECTF_HPP

#include <assert.h>
#include <iosfwd>

#include <SDL.h>

#include "math/anchor_point.hpp"
#include "math/sizef.hpp"
#include "math/vector.hpp"

class Rect;

class Rectf final
{
public:
  static Rectf from_center(const Vector& center, const Sizef& size)
  {
    return Rectf(center.x - size.width / 2.0f,
                 center.y - size.height / 2.0f,
                 center.x + size.width / 2.0f,
                 center.y + size.height / 2.0f);
  }

public:
  Rectf() :
    m_p1(0.0f, 0.0f),
    m_size()
  { }

  Rectf(const Rectf& rhs) = default;
  Rectf& operator=(const Rectf& rhs) = default;

  Rectf(const Vector& np1, const Vector& np2) :
    m_p1(np1), m_size(np2.x - np1.x, np2.y - np1.y)
  {
    assert(m_size.width >= 0 &&
           m_size.height >= 0);
  }

  Rectf(float x1, float y1, float x2, float y2) :
    m_p1(x1, y1), m_size(x2 - x1, y2 - y1)
  {
    assert(m_size.width >= 0 &&
           m_size.height >= 0);
  }

  Rectf(const Vector& p1, const Sizef& size) :
    m_p1(p1),
    m_size(size)
  {
  }

  Rectf(const SDL_FRect& rect) :
    m_p1(rect.x, rect.y), m_size(rect.w, rect.h)
  {
  }

  Rectf(const Rect& rect);

  bool operator==(const Rectf& other) const
  {
    return (m_p1 == other.m_p1 &&
            m_size == other.m_size);
  }

  inline float& get_left() { return m_p1.x; }
  inline float& get_top() { return m_p1.y; }

  inline float get_left() const { return m_p1.x; }
  inline float get_right() const { return m_p1.x + m_size.width; }
  inline float get_top() const { return m_p1.y; }
  inline float get_bottom() const { return m_p1.y + m_size.height; }

  inline float& get_width() { return m_size.width; }
  inline float& get_height() { return m_size.height; }

  inline float get_width() const { return m_size.width; }
  inline float get_height() const { return m_size.height; }

  inline void set_left(float v) { m_size.width -= v - m_p1.x; m_p1.x = v; }
  inline void set_right(float v) { m_size.width += v - get_right(); }

  inline void set_top(float v) { m_size.height -= v - m_p1.y; m_p1.y = v; }
  inline void set_bottom(float v) { m_size.height += v - get_bottom(); }

  Vector get_middle() const { return Vector(m_p1.x + m_size.width / 2.0f,
                                            m_p1.y + m_size.height / 2.0f); }

  inline void set_pos(const Vector& v) { m_p1 = v; }

  inline void set_width(float width) { m_size.width = width; }
  inline void set_height(float height) { m_size.height = height; }
  inline void set_size(float width, float height) { m_size = Sizef(width, height); }
  inline Sizef get_size() const { return m_size; }

  bool empty() const
  {
    return get_width() <= 0 ||
           get_height() <= 0;
  }

  inline void move(const Vector& v) { m_p1 += v; }
  Rectf moved(const Vector& v) const { return Rectf(m_p1 + v, m_size); }

  bool contains(const Vector& v) const {
    return v.x >= m_p1.x && v.y >= m_p1.y && v.x < get_right() && v.y < get_bottom();
  }

  bool overlaps(const Rectf& other) const
  {
    if (get_right() < other.get_left() || get_left() > other.get_right())
      return false;
    if (get_bottom() < other.get_top() || get_top() > other.get_bottom())
      return false;

    return true;
  }

  float distance (const Vector& other, AnchorPoint ap = ANCHOR_MIDDLE) const
  {
    Vector v = get_anchor_pos (*this, ap);
    return glm::distance(v, other);
  }

  float distance (const Rectf& other, AnchorPoint ap = ANCHOR_MIDDLE) const
  {
    Vector v1 = get_anchor_pos(*this, ap);
    Vector v2 = get_anchor_pos(other, ap);

    return glm::distance(v1, v2);
  }

  Rectf grown(float border) const
  {
    // If the size would be shrunk below 0, do not resize.
    if (m_size.width + border * 2 < 0.f || m_size.height + border * 2 < 0.f)
      return *this;

    return Rectf(m_p1.x - border, m_p1.y - border,
                 get_right() + border, get_bottom() + border);
  }

  // leave these two public to save the headaches of set/get functions for such
  // simple things :)

  inline const Vector& p1() const { return m_p1; }
  inline Vector p2() const { return Vector(m_p1.x + m_size.width, m_p1.y + m_size.height); }

  void set_p1(const Vector& p) {
    m_size = Sizef(m_size.width + (m_p1.x - p.x),
                   m_size.height + (m_p1.y - p.y));
    m_p1 = p;
  }
  void set_p2(const Vector& p) {
    m_size = Sizef(p.x - m_p1.x,
                   p.y - m_p1.y);
  }

  Rect to_rect() const;
  SDL_FRect to_sdl() const
  {
    return { m_p1.x, m_p1.y, m_size.width, m_size.height };
  }

private:
  /// upper left edge
  Vector m_p1;
  Sizef m_size;
};

std::ostream& operator<<(std::ostream& out, const Rectf& rect);

#endif

/* EOF */
