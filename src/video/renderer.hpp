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

#ifndef HEADER_SUPERTUX_VIDEO_RENDERER_HPP
#define HEADER_SUPERTUX_VIDEO_RENDERER_HPP

#include "math/rect.hpp"
#include "math/vector.hpp"
#include "video/color.hpp"
#include "video/texture_ptr.hpp"

class Painter;
class Rect;
struct DrawingRequest;
struct SDL_Window;

class Renderer
{
public:
  virtual ~Renderer() {}

  virtual void start_draw() = 0;
  virtual void end_draw() = 0;

  virtual Painter& get_painter() = 0;

  virtual Rect get_rect() const = 0;
  virtual Size get_logical_size() const = 0;

  virtual TexturePtr get_texture() const = 0;
};

#endif

/* EOF */
