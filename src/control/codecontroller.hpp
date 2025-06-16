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

#ifndef HEADER_SUPERTUX_CONTROL_CODECONTROLLER_HPP
#define HEADER_SUPERTUX_CONTROL_CODECONTROLLER_HPP

#include "control/controller.hpp"

/** This is a dummy controller that doesn't react to any user input but should
    be controlled by code */
class CodeController final : public Controller
{
public:
  CodeController();
  ~CodeController() override;

  virtual void update() override;

  void press(Control c, bool pressed = true);

private:
  CodeController(const CodeController&) = delete;
  CodeController& operator=(const CodeController&) = delete;
};

#endif

/* EOF */
