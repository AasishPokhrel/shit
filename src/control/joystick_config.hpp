//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_CONTROL_JOYSTICK_CONFIG_HPP
#define HEADER_SUPERTUX_CONTROL_JOYSTICK_CONFIG_HPP

#include <map>
#include <SDL.h>

#include "control/controller.hpp"

class ReaderMapping;
class Writer;

class JoystickConfig final
{
  friend class InputManager;
  friend class JoystickManager;
  friend class JoystickMenu;

public:
  using JoystickID = SDL_JoystickID;

public:
  JoystickConfig();

  void print_joystick_mappings() const;

  int reversemap_joybutton(Control c) const;
  int reversemap_joyaxis(Control c) const;
  int reversemap_joyhat(Control c) const;

  void unbind_joystick_control(Control c);

  void bind_joybutton(JoystickID joy_id, int button, Control c);
  void bind_joyaxis(JoystickID joy_id, int axis, Control c);
  void bind_joyhat(JoystickID joy_id, int dir, Control c);

  void read(const ReaderMapping& joystick_mapping);
  void write(Writer& writer);

private:
  int m_dead_zone;
  bool m_jump_with_up_joy;
  bool m_use_game_controller;

  std::map<std::pair<JoystickID, int>, Control> m_joy_button_map;
  std::map<std::pair<JoystickID, int>, Control> m_joy_axis_map;
  std::map<std::pair<JoystickID, int>, Control> m_joy_hat_map;

private:
  JoystickConfig(const JoystickConfig&) = delete;
  JoystickConfig& operator=(const JoystickConfig&) = delete;
};

#endif

/* EOF */
