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

#ifndef HEADER_SUPERTUX_EDITOR_TOOL_ICON_HPP
#define HEADER_SUPERTUX_EDITOR_TOOL_ICON_HPP

#include "video/surface_ptr.hpp"
#include "math/vector.hpp"

#include <string>
#include <vector>

class DrawingContext;

class ToolIcon final
{
public:
  ToolIcon(const std::string& icon);

  void draw(DrawingContext& context);

  inline int get_mode() const { return m_mode; }

  void next_mode();

  void push_mode(const std::string& icon);
  SurfacePtr get_current_surface() const;

public:
  Vector m_pos;

private:
  std::vector<SurfacePtr> m_surfaces;
  int m_mode;
  int m_surf_count;

private:
  ToolIcon(const ToolIcon&) = delete;
  ToolIcon& operator=(const ToolIcon&) = delete;
};

#endif

/* EOF */
