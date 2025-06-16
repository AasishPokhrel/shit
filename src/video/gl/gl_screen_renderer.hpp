//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_SCREEN_RENDERER_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_SCREEN_RENDERER_HPP

#include <SDL.h>

#include "math/vector.hpp"
#include "video/gl/gl_renderer.hpp"

class GLVideoSystem;
struct DrawingRequest;

class GLScreenRenderer final : public GLRenderer
{
public:
  GLScreenRenderer(GLVideoSystem& video_system);
  ~GLScreenRenderer() override;

  virtual void start_draw() override;
  virtual void end_draw() override;

  virtual Rect get_rect() const override;
  virtual Size get_logical_size() const override;

  virtual TexturePtr get_texture() const override { return {}; }

private:
  GLScreenRenderer(const GLScreenRenderer&) = delete;
  GLScreenRenderer& operator=(const GLScreenRenderer&) = delete;
};

#endif

/* EOF */
