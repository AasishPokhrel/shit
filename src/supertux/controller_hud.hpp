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

#ifndef HEADER_SUPERTUX_SUPERTUX_CONTROLLER_HUD_HPP
#define HEADER_SUPERTUX_SUPERTUX_CONTROLLER_HUD_HPP

#include <map>

#include "control/controller.hpp"
#include "math/rectf.hpp"

class DrawingContext;

class ControllerHUD
{
public:
  ControllerHUD();

  void draw(DrawingContext& context);

private:
  std::map<Control, Rectf> m_controls;

private:
  ControllerHUD(const ControllerHUD&) = delete;
  ControllerHUD& operator=(const ControllerHUD&) = delete;
};

#endif

/* EOF */
