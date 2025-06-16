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

#ifndef HEADER_SUPERTUX_CONTROL_INPUT_MANAGER_HPP
#define HEADER_SUPERTUX_CONTROL_INPUT_MANAGER_HPP

#include "control/controller.hpp"

#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "util/currenton.hpp"

class GameControllerManager;
class JoystickManager;
class JoystickMenu;
class KeyboardManager;
class KeyboardMenu;
class KeyboardConfig;
class JoystickConfig;

class InputManager final : public Currenton<InputManager>
{
private:
  friend class KeyboardMenu;
  friend class JoystickMenu;

public:
  InputManager(KeyboardConfig& keyboard_config,
               JoystickConfig& joystick_config);
  ~InputManager() override;

  void process_event(const SDL_Event& event);

  void update();
  void reset();

  void use_game_controller(bool v);
  inline bool use_game_controller() const { return m_use_game_controller; }

  const Controller& get_controller(int player_id = 0) const;
  Controller& get_controller(int player_id = 0);

  inline int get_num_users() const { return static_cast<int>(m_controllers.size()); }
  bool can_add_user() const;
  void push_user();
  void pop_user();

  void on_player_removed(int player_id);

  bool has_corresponsing_controller(int player_id) const;

private:
  std::vector<std::unique_ptr<Controller>> m_controllers;

public:
  bool& m_use_game_controller;
  std::unique_ptr<KeyboardManager> keyboard_manager;
  std::unique_ptr<JoystickManager> joystick_manager;
  std::unique_ptr<GameControllerManager> game_controller_manager;

  /** True if the given player is on the keyboard and plays without a controller */
  std::unordered_map<int, bool> m_uses_keyboard;

private:
  InputManager(const InputManager&) = delete;
  InputManager& operator=(const InputManager&) = delete;
};

#endif

/* EOF */
