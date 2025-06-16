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

#include "control/joystick_config.hpp"

#include <iostream>

#include "util/gettext.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

JoystickConfig::JoystickConfig() :
  m_dead_zone(8000),
  m_jump_with_up_joy(false),
  m_use_game_controller(true),
  m_joy_button_map(),
  m_joy_axis_map(),
  m_joy_hat_map()
{
  // Default joystick button configuration
  bind_joybutton(0, 0, Control::JUMP);
  bind_joybutton(0, 0, Control::MENU_SELECT);
  bind_joybutton(0, 1, Control::ACTION);
  bind_joybutton(0, 4, Control::PEEK_LEFT);
  bind_joybutton(0, 5, Control::PEEK_RIGHT);
  bind_joybutton(0, 6, Control::START);
  bind_joybutton(0, 7, Control::ITEM);

  // Default joystick axis configuration
  bind_joyaxis(0, -1, Control::LEFT);
  bind_joyaxis(0, 1, Control::RIGHT);
  bind_joyaxis(0, -2, Control::UP);
  bind_joyaxis(0, 2, Control::DOWN);
}

int
JoystickConfig::reversemap_joyaxis(Control c) const
{
  for (const auto& i : m_joy_axis_map) {
    if (i.second == c)
      return i.first.second;
  }

  return 0;
}

int
JoystickConfig::reversemap_joybutton(Control c) const
{
  for (const auto& i : m_joy_button_map) {
    if (i.second == c)
      return i.first.second;
  }

  return -1;
}

int
JoystickConfig::reversemap_joyhat(Control c) const
{
  for (const auto& i : m_joy_hat_map) {
    if (i.second == c)
      return i.first.second;
  }

  return -1;
}

void
JoystickConfig::print_joystick_mappings() const
{
  std::cout << _("Joystick Mappings") << std::endl;
  std::cout << "-----------------" << std::endl;
  for (const auto& i : m_joy_axis_map) {
    std::cout << "Axis: " << i.first.second << " -> " << i.second << std::endl;
  }

  for (const auto& i : m_joy_button_map) {
    std::cout << "Button: " << i.first.second << " -> " << i.second << std::endl;
  }

  for (const auto& i : m_joy_hat_map) {
    std::cout << "Hat: " << i.first.second << " -> " << i.second << std::endl;
  }
  std::cout << std::endl;
}

void
JoystickConfig::unbind_joystick_control(Control control)
{
  // remove all previous mappings for that control
  for (auto i = m_joy_axis_map.begin(); i != m_joy_axis_map.end(); /* no ++i */) {
    if (i->second == control)
      m_joy_axis_map.erase(i++);
    else
      ++i;
  }

  for (auto i = m_joy_button_map.begin(); i != m_joy_button_map.end(); /* no ++i */) {
    if (i->second == control)
      m_joy_button_map.erase(i++);
    else
      ++i;
  }

  for (auto i = m_joy_hat_map.begin(); i != m_joy_hat_map.end(); /* no ++i */) {
    if (i->second == control)
      m_joy_hat_map.erase(i++);
    else
      ++i;
  }
}

void
JoystickConfig::bind_joyaxis(JoystickID joy_id, int axis, Control control)
{
  // axis isn't the SDL axis number, but axisnumber + 1 with sign
  // changed depending on if the positive or negative end is to be
  // used (negative axis 0 becomes -1, positive axis 2 becomes +3,
  // etc.)

  unbind_joystick_control(control);

  // add new mapping
  m_joy_axis_map[std::make_pair(joy_id, axis)] = control;
}

void
JoystickConfig::bind_joyhat(JoystickID joy_id, int dir, Control c)
{
  unbind_joystick_control(c);

  // add new mapping
  m_joy_hat_map[std::make_pair(joy_id, dir)] = c;
}

void
JoystickConfig::bind_joybutton(JoystickID joy_id, int button, Control control)
{
  unbind_joystick_control(control);

  // add new mapping
  m_joy_button_map[std::make_pair(joy_id, button)] = control;
}

void
JoystickConfig::read(const ReaderMapping& joystick_mapping)
{
  joystick_mapping.get("dead-zone", m_dead_zone);
  joystick_mapping.get("jump-with-up", m_jump_with_up_joy);
  joystick_mapping.get("use-game-controller", m_use_game_controller);

  auto iter = joystick_mapping.get_iter();
  while (iter.next())
  {
    if (iter.get_key() == "map")
    {
      const auto& map = iter.as_mapping();

      std::string control_text;
      map.get("control", control_text);

      const std::optional<Control> maybe_control = Control_from_string(control_text);
      if (!maybe_control)
      {
        log_info << "Invalid control '" << control_text << "' in buttonmap" << std::endl;
      }
      else
      {
        const Control control = *maybe_control;

        int button = -1;
        int axis   = 0;
        int hat    = -1;

        if (map.get("button", button))
        {
          bind_joybutton(0, button, control);
        }
        else if (map.get("axis",   axis))
        {
          bind_joyaxis(0, axis, control);
        }
        else if (map.get("hat",   hat))
        {
          if (hat != SDL_HAT_UP   &&
              hat != SDL_HAT_DOWN &&
              hat != SDL_HAT_LEFT &&
              hat != SDL_HAT_RIGHT) {
            log_info << "Invalid axis '" << axis << "' in axismap" << std::endl;
          }
          else
          {
            bind_joyhat(0, hat, control);
          }
        }
      }
    }
  }
}

void
JoystickConfig::write(Writer& writer)
{
  writer.write("dead-zone", m_dead_zone);
  writer.write("jump-with-up", m_jump_with_up_joy);
  writer.write("use-game-controller", m_use_game_controller);

  for (const auto& i : m_joy_button_map) {
    writer.start_list("map");
    writer.write("button", i.first.second);
    writer.write("control", Control_to_string(i.second));
    writer.end_list("map");
  }

  for (const auto& i : m_joy_hat_map) {
    writer.start_list("map");
    writer.write("hat", i.first.second);
    writer.write("control", Control_to_string(i.second));
    writer.end_list("map");
  }

  for (const auto& i : m_joy_axis_map) {
    writer.start_list("map");
    writer.write("axis", i.first.second);
    writer.write("control", Control_to_string(i.second));
    writer.end_list("map");
  }
}

/* EOF */
