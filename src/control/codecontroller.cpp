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

#include "control/codecontroller.hpp"

CodeController::CodeController()
{}

CodeController::~CodeController()
{}

void
CodeController::press(Control c, bool pressed_)
{
  m_controls[static_cast<int>(c)] = pressed_;
}

void
CodeController::update()
{
  Controller::update();

  for (int i = 0; i < static_cast<int>(Control::CONTROLCOUNT); ++i) {
    m_controls[i] = false;
  }
}

/* EOF */
