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

#ifndef HEADER_SUPERTUX_CONTROL_GAME_CONTROLLER_MANAGER_HPP
#define HEADER_SUPERTUX_CONTROL_GAME_CONTROLLER_MANAGER_HPP

#include <array>
#include <vector>
#include <unordered_map>

#include "control/controller.hpp"

class InputManager;
struct SDL_ControllerAxisEvent;
struct SDL_ControllerButtonEvent;
struct _SDL_GameController;
typedef struct _SDL_GameController SDL_GameController;

/**
 * Manages GameControllers.
 * 
 * WARNING: Any edit done to this class should also be done to JoystickManager!
 */
class GameControllerManager final
{
public:
  GameControllerManager(InputManager* parent);
  ~GameControllerManager();

  void process_button_event(const SDL_ControllerButtonEvent& ev);
  void process_axis_event(const SDL_ControllerAxisEvent& ev);

  void on_controller_added(int joystick_index);
  void on_controller_removed(int instance_id);

  void on_player_removed(int player_id);
  bool has_corresponding_game_controller(int player_id) const;

  /** @returns 0 if success, 1 if controller doesn't support rumbling, 2 if game doesn't support rumbling */
  int rumble(SDL_GameController* controller) const;

  void bind_controller(SDL_GameController* controller, int player_id);

  inline std::unordered_map<SDL_GameController*, int>& get_controller_mapping() { return m_game_controllers; }

private:
  InputManager* m_parent;
  int m_deadzone;
  std::unordered_map<SDL_GameController*, int> m_game_controllers;
  std::array<bool, static_cast<int>(Control::CONTROLCOUNT)> m_stick_state;
  std::array<bool, static_cast<int>(Control::CONTROLCOUNT)> m_button_state;

private:
  GameControllerManager(const GameControllerManager&) = delete;
  GameControllerManager& operator=(const GameControllerManager&) = delete;
};

#endif

/* EOF */
