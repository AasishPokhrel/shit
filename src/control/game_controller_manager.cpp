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

#include "control/game_controller_manager.hpp"

#include <algorithm>

#include "control/input_manager.hpp"
#include "object/player.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/game_session.hpp"
#include "supertux/savegame.hpp"
#include "util/log.hpp"

GameControllerManager::GameControllerManager(InputManager* parent) :
  m_parent(parent),
  m_deadzone(8000),
  m_game_controllers(),
  m_stick_state(),
  m_button_state()
{
}

GameControllerManager::~GameControllerManager()
{
  for (const auto& con : m_game_controllers)
  {
    SDL_GameControllerClose(con.first);
  }
}

void
GameControllerManager::process_button_event(const SDL_ControllerButtonEvent& ev)
{
  int player_id;

  {
    auto it = m_game_controllers.find(SDL_GameControllerFromInstanceID(ev.which));

    if (it == m_game_controllers.end() || it->second < 0)
      return;

    player_id = it->second;
  }

  //log_info << "button event: " << static_cast<int>(ev.button) << " " << static_cast<int>(ev.state) << std::endl;
  Controller& controller = m_parent->get_controller(player_id);
  auto set_control = [this, &controller](Control control, Uint8 value)
  {
    m_button_state[static_cast<int>(control)] = (value != 0);
    controller.set_control(control, m_button_state[static_cast<int>(control)] == SDL_PRESSED || m_stick_state[static_cast<int>(control)] == SDL_PRESSED);
  };
  switch (ev.button)
  {
    case SDL_CONTROLLER_BUTTON_A:
      set_control(Control::JUMP, ev.state);
      set_control(Control::MENU_SELECT, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_B:
      set_control(Control::MENU_BACK, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_X:
      set_control(Control::ACTION, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_Y:
      break;

    case SDL_CONTROLLER_BUTTON_BACK:
      set_control(Control::ITEM, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_GUIDE:
      set_control(Control::CHEAT_MENU, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_START:
      set_control(Control::START, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
      break;

    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
      break;

    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
      set_control(Control::PEEK_LEFT, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
      set_control(Control::PEEK_RIGHT, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      set_control(Control::UP, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      set_control(Control::DOWN, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
      set_control(Control::LEFT, ev.state);
      break;

    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
      set_control(Control::RIGHT, ev.state);
      break;

    default:
      break;
  }
}

void
GameControllerManager::process_axis_event(const SDL_ControllerAxisEvent& ev)
{
  int player_id;

  {
    auto it = m_game_controllers.find(SDL_GameControllerFromInstanceID(ev.which));

    if (it == m_game_controllers.end() || it->second < 0)
      return;

    player_id = it->second;
  }

  // FIXME: Buttons and axis are fighting for control ownership, need jump slightly if we encounter a suitable totem.

  //log_info << "axis event: " << static_cast<int>(ev.axis) << " " << ev.value << std::endl;
  Controller& controller = m_parent->get_controller(player_id);
  auto set_control = [this, &controller](Control control, bool value)
  {
    // Check if the input hasn't been changed by anything else like the keyboard.
    if (controller.hold(control) == m_stick_state[static_cast<int>(control)] &&
        controller.hold(control) != value)
    {
      m_stick_state[static_cast<int>(control)] = value;
      bool newstate = m_button_state[static_cast<int>(control)] || m_stick_state[static_cast<int>(control)];
      controller.set_control(control, newstate);
    }
  };

  auto axis2button = [this, &set_control](int value, Control control_left, Control control_right)
    {
      if (value < -m_deadzone)
      {
        set_control(control_left, true);
        set_control(control_right, false);
      }
      else if (value > m_deadzone)
      {
        set_control(control_left, false);
        set_control(control_right, true);
      }
      else
      {
        set_control(control_left, false);
        set_control(control_right, false);
      }
    };

  switch (ev.axis)
  {
    case SDL_CONTROLLER_AXIS_LEFTX:
      axis2button(ev.value, Control::LEFT, Control::RIGHT);
      break;

    case SDL_CONTROLLER_AXIS_LEFTY:
      axis2button(ev.value, Control::UP, Control::DOWN);
      break;

    case SDL_CONTROLLER_AXIS_RIGHTX:
      axis2button(ev.value, Control::PEEK_LEFT, Control::PEEK_RIGHT);
      break;

    case SDL_CONTROLLER_AXIS_RIGHTY:
      axis2button(ev.value, Control::PEEK_UP, Control::PEEK_DOWN);
      break;

    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
      break;

    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
      break;

    default:
      break;
  }
}

void
GameControllerManager::on_controller_added(int joystick_index)
{
  if (!m_parent->can_add_user())
    return;

  if (!SDL_IsGameController(joystick_index))
  {
    log_warning << "joystick is not a game controller, ignoring: " << joystick_index << std::endl;
  }
  else
  {
    SDL_GameController* game_controller = SDL_GameControllerOpen(joystick_index);
    if (!game_controller)
    {
      log_warning << "failed to open game_controller: " << joystick_index
                  << ": " << SDL_GetError() << std::endl;
    }
    else
    {
      m_game_controllers[game_controller] = -1;

      if (m_parent->m_use_game_controller && g_config->multiplayer_auto_manage_players)
      {
        int id = m_parent->get_num_users();
        for (int i = 0; i < m_parent->get_num_users(); i++)
        {
          if (!m_parent->has_corresponsing_controller(i) && !m_parent->m_uses_keyboard[i])
          {
            id = i;
            break;
          }
        }

        if (id == m_parent->get_num_users())
          m_parent->push_user();

        m_game_controllers[game_controller] = id;

        if (GameSession::current() && !GameSession::current()->get_savegame().is_title_screen() && id != 0)
        {
          GameSession::current()->on_player_added(id);
        }
      }
    }
  }
}

void
GameControllerManager::on_controller_removed(int instance_id)
{
  auto it = std::find_if(m_game_controllers.begin(), m_game_controllers.end(), [instance_id] (decltype(m_game_controllers)::const_reference pair) {
    return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pair.first)) == instance_id;
  });

  if (it != m_game_controllers.end())
  {
    SDL_GameControllerClose(it->first);

    auto deleted_player_id = it->second;

    m_game_controllers.erase(it);

    if (m_parent->m_use_game_controller && g_config->multiplayer_auto_manage_players
        && deleted_player_id != 0 && !m_parent->m_uses_keyboard[deleted_player_id] &&
        GameSession::current())
    {
      GameSession::current()->on_player_removed(deleted_player_id);
    }
  }
  else
  {
    log_debug << "Controller was unplugged but was not initially detected: "
              << SDL_JoystickName(SDL_JoystickFromInstanceID(instance_id))
              << std::endl;
  }
}

void
GameControllerManager::on_player_removed(int player_id)
{
  auto it2 = std::find_if(m_game_controllers.begin(), m_game_controllers.end(), [player_id](decltype(m_game_controllers)::const_reference pair) {
    return pair.second == player_id;
  });
  if (it2 != m_game_controllers.end())
  {
    it2->second = -1;
    // Try again, in case multiple controllers were bount to a player.
    // Recursive call shouldn't go too deep except in hardcore scenarios.
    on_player_removed(player_id);
  }
}

bool
GameControllerManager::has_corresponding_game_controller(int player_id) const
{
  return std::find_if(m_game_controllers.begin(), m_game_controllers.end(), [player_id](decltype(m_game_controllers)::const_reference pair) {
    return pair.second == player_id;
  }) != m_game_controllers.end();
}

int
GameControllerManager::rumble(SDL_GameController* controller) const
{
#if SDL_VERSION_ATLEAST(2, 0, 9)
  if (g_config->multiplayer_buzz_controllers)
  {
#if SDL_VERSION_ATLEAST(2, 0, 18)
    if (SDL_GameControllerHasRumble(controller))
    {
#endif
      // TODO: Rumble intensity setting (like volume).
      SDL_GameControllerRumble(controller, 0xFFFF, 0xFFFF, 300);
#if SDL_VERSION_ATLEAST(2, 0, 18)
    }
    else
    {
      return 1;
    }
#endif
  }

  return 0;
#else
  return 2;
#endif
}

void
GameControllerManager::bind_controller(SDL_GameController* controller, int player_id)
{
  m_game_controllers[controller] = player_id;

  if (!g_config->multiplayer_multibind)
    for (auto& pair2 : m_game_controllers)
      if (pair2.second == player_id && pair2.first != controller)
        pair2.second = -1;
}

/* EOF */
