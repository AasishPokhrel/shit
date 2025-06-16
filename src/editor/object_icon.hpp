//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_ICON_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_ICON_HPP

#include <string>

#include "math/vector.hpp"
#include "video/surface_ptr.hpp"

class DrawingContext;
class ReaderMapping;

class ObjectIcon
{
public:
  ObjectIcon(const std::string& name, const std::string& icon);
  ObjectIcon(const ReaderMapping& reader);
  virtual ~ObjectIcon();

  virtual void draw(DrawingContext& context, const Vector& pos);
  virtual void draw(DrawingContext& context, const Vector& pos, int pixels_shown);

  inline const std::string& get_object_class() const { return m_object_class; }

  ObjectIcon(const ObjectIcon&) = default;
  ObjectIcon& operator=(const ObjectIcon&) = default;

private:
  void calculate_offset();

private:
  std::string m_object_class;
  SurfacePtr m_surface;
  Vector m_offset;
};

#endif

/* EOF */
