//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
//                2007-2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_CONTROL_KEYBOARD_MANAGER_HPP
#define HEADER_SUPERTUX_CONTROL_KEYBOARD_MANAGER_HPP

#include <optional>

#include "control/controller.hpp"
#include "control/keyboard_config.hpp"

class InputManager;
struct SDL_KeyboardEvent;
struct SDL_TextInputEvent;

class KeyboardManager final
{
public:
  KeyboardManager(InputManager* parent, KeyboardConfig& keyboard_config);

  void process_key_event(const SDL_KeyboardEvent& event);
  void process_text_input_event(const SDL_TextInputEvent& event);
  void process_console_key_event(const SDL_KeyboardEvent& event);
  void process_menu_key_event(const SDL_KeyboardEvent& event);

  void bind_next_event_to(int player_id, Control id);

private:
  InputManager* m_parent;
  KeyboardConfig& m_keyboard_config;
  std::optional<KeyboardConfig::PlayerControl> m_wait_for_key;
  bool m_lock_text_input;

private:
  KeyboardManager(const KeyboardManager&) = delete;
  KeyboardManager& operator=(const KeyboardManager&) = delete;
};

#endif

/* EOF */
