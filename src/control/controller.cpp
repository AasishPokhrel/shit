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

#include "control/controller.hpp"

#include <ostream>

namespace {

const char* g_control_names[] = {
  "left",
  "right",
  "up",
  "down",
  "jump",
  "action",
  "item",
  "start",
  "escape",
  "menu-select",
  "menu-select-space",
  "menu-back",
  "remove",
  "cheat-menu",
  "debug-menu",
  "console",
  "peek-left",
  "peek-right",
  "peek-up",
  "peek-down",
  nullptr
};

} // namespace

std::ostream& operator<<(std::ostream& os, Control control)
{
  return os << g_control_names[static_cast<int>(control)];
}

std::string Control_to_string(Control control)
{
  return g_control_names[static_cast<int>(control)];
}

std::optional<Control> Control_from_string(const std::string& text)
{
  for(int i = 0; g_control_names[i] != nullptr; ++i) {
    if (text == g_control_names[i]) {
      return static_cast<Control>(i);
    }
  }

  return std::nullopt;
}

Controller::Controller():
  m_touchscreen(false),
  m_jump_key_pressed(false)
{
  reset();
}

Controller::~Controller()
{}

void
Controller::reset()
{
  for (int i = 0; i < static_cast<int>(Control::CONTROLCOUNT); ++i) {
    m_controls[i] = false;
    m_old_controls[i] = false;
  }
  m_touchscreen = false;
  m_jump_key_pressed = false;
}

void
Controller::set_control(Control control, bool value)
{
  if (control == Control::JUMP) {
    m_jump_key_pressed = value;
  }
  m_controls[static_cast<int>(control)] = value;
}

void
Controller::set_jump_key_with_up(bool value)
{
  // Do not release the jump key if the jump key is still pressed.
  if (!m_jump_key_pressed) {
    m_controls[static_cast<int>(Control::JUMP)] = value;
  }
}

bool
Controller::hold(Control control) const
{
  return m_controls[static_cast<int>(control)];
}

bool
Controller::pressed(Control control) const
{
  return !m_old_controls[static_cast<int>(control)] && m_controls[static_cast<int>(control)];
}

bool
Controller::released(Control control) const
{
  return m_old_controls[static_cast<int>(control)] && !m_controls[static_cast<int>(control)];
}

void
Controller::update()
{
  for (int i = 0; i < static_cast<int>(Control::CONTROLCOUNT); ++i) {
    m_old_controls[i] = m_controls[i];
  }
}

/* EOF */
