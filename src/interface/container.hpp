//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTAINER_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTAINER_HPP

#include <SDL.h>

#include "interface/control.hpp"
#include "math/rectf.hpp"
#include "video/drawing_context.hpp"

/** Class that's designed to contain other controls */
class InterfaceContainer final : public InterfaceControl
{
public:
  InterfaceContainer();
  virtual ~InterfaceContainer() override {}

  virtual bool event(const SDL_Event& ev) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

public:
  std::vector<InterfaceControl> m_children;

private:
  InterfaceContainer(const InterfaceContainer&) = delete;
  InterfaceContainer& operator=(const InterfaceContainer&) = delete;
};

#endif

/* EOF */
