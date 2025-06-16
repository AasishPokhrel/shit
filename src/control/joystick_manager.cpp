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

#include "control/joystick_manager.hpp"

#include <algorithm>

#include "control/input_manager.hpp"
#include "control/joystick_config.hpp"
#include "gui/menu_manager.hpp"
#include "object/player.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/game_session.hpp"
#include "supertux/savegame.hpp"
#include "util/log.hpp"

JoystickManager::JoystickManager(InputManager* parent_,
                                 JoystickConfig& joystick_config) :
  parent(parent_),
  m_joystick_config(joystick_config),
  min_joybuttons(),
  max_joybuttons(),
  max_joyaxis(),
  max_joyhats(),
  hat_state(0),
  wait_for_joystick(-1),
  joysticks()
{
}

JoystickManager::~JoystickManager()
{
  for (auto& joy : joysticks)
  {
    SDL_JoystickClose(joy.first);
  }
}

void
JoystickManager::on_joystick_added(int joystick_index)
{
  log_debug << "on_joystick_added(): " << joystick_index << std::endl;

  if (!parent->can_add_user())
    return;

  SDL_Joystick* joystick = SDL_JoystickOpen(joystick_index);
  if (!joystick)
  {
    log_warning << "failed to open joystick: " << joystick_index
                << ": " << SDL_GetError() << std::endl;
  }
  else
  {
    joysticks[joystick] = -1;

    if (min_joybuttons < 0 || SDL_JoystickNumButtons(joystick) < min_joybuttons)
      min_joybuttons = SDL_JoystickNumButtons(joystick);

    if (SDL_JoystickNumButtons(joystick) > max_joybuttons)
      max_joybuttons = SDL_JoystickNumButtons(joystick);

    if (SDL_JoystickNumAxes(joystick) > max_joyaxis)
      max_joyaxis = SDL_JoystickNumAxes(joystick);

    if (SDL_JoystickNumHats(joystick) > max_joyhats)
      max_joyhats = SDL_JoystickNumHats(joystick);

    if (!parent->m_use_game_controller && g_config->multiplayer_auto_manage_players)
    {
      int id = parent->get_num_users();
      for (int i = 0; i < parent->get_num_users(); i++)
      {
        if (!parent->has_corresponsing_controller(i) && !parent->m_uses_keyboard[i])
        {
          id = i;
          break;
        }
      }

      if (id == parent->get_num_users())
        parent->push_user();

      joysticks[joystick] = id;

      if (GameSession::current() && !GameSession::current()->get_savegame().is_title_screen() && id != 0)
      {
        GameSession::current()->on_player_added(id);
      }
    }
  }
}

void
JoystickManager::on_joystick_removed(int instance_id)
{
  log_debug << "on_joystick_removed: " << static_cast<int>(instance_id) << std::endl;

  auto it = std::find_if(joysticks.begin(), joysticks.end(), [instance_id] (decltype(joysticks)::const_reference pair) {
    return SDL_JoystickInstanceID(pair.first) == instance_id;
  });

  if (it != joysticks.end())
  {
    SDL_JoystickClose(it->first);

    auto deleted_player_id = it->second;

    joysticks.erase(it);

    if (!parent->m_use_game_controller && g_config->multiplayer_auto_manage_players
        && deleted_player_id != 0 && !parent->m_uses_keyboard[deleted_player_id] &&
        GameSession::current())
    {
      GameSession::current()->on_player_removed(deleted_player_id);
    }
  }
  else
  {
    log_debug << "Joystick was unplugged but was not initially detected: "
              << SDL_JoystickName(SDL_JoystickFromInstanceID(instance_id))
              << std::endl;
  }
}

void
JoystickManager::process_hat_event(const SDL_JoyHatEvent& jhat)
{
  Uint8 changed = hat_state ^ jhat.value;

  if (wait_for_joystick >= 0)
  {
    if (changed & SDL_HAT_UP && jhat.value & SDL_HAT_UP)
      m_joystick_config.bind_joyhat(jhat.which, SDL_HAT_UP, Control(wait_for_joystick));

    if (changed & SDL_HAT_DOWN && jhat.value & SDL_HAT_DOWN)
      m_joystick_config.bind_joyhat(jhat.which, SDL_HAT_DOWN, Control(wait_for_joystick));

    if (changed & SDL_HAT_LEFT && jhat.value & SDL_HAT_LEFT)
      m_joystick_config.bind_joyhat(jhat.which, SDL_HAT_LEFT, Control(wait_for_joystick));

    if (changed & SDL_HAT_RIGHT && jhat.value & SDL_HAT_RIGHT)
      m_joystick_config.bind_joyhat(jhat.which, SDL_HAT_RIGHT, Control(wait_for_joystick));

    MenuManager::instance().refresh();
    wait_for_joystick = -1;
  }
  else
  {
    if (changed & SDL_HAT_UP)
    {
      auto it = m_joystick_config.m_joy_hat_map.find(std::make_pair(jhat.which, SDL_HAT_UP));
      if (it != m_joystick_config.m_joy_hat_map.end())
        set_joy_controls(jhat.which, it->second, (jhat.value & SDL_HAT_UP) != 0);
    }

    if (changed & SDL_HAT_DOWN)
    {
      auto it = m_joystick_config.m_joy_hat_map.find(std::make_pair(jhat.which, SDL_HAT_DOWN));
      if (it != m_joystick_config.m_joy_hat_map.end())
        set_joy_controls(jhat.which, it->second, (jhat.value & SDL_HAT_DOWN) != 0);
    }

    if (changed & SDL_HAT_LEFT)
    {
      auto it = m_joystick_config.m_joy_hat_map.find(std::make_pair(jhat.which, SDL_HAT_LEFT));
      if (it != m_joystick_config.m_joy_hat_map.end())
        set_joy_controls(jhat.which, it->second, (jhat.value & SDL_HAT_LEFT) != 0);
    }

    if (changed & SDL_HAT_RIGHT)
    {
      auto it = m_joystick_config.m_joy_hat_map.find(std::make_pair(jhat.which, SDL_HAT_RIGHT));
      if (it != m_joystick_config.m_joy_hat_map.end())
        set_joy_controls(jhat.which, it->second, (jhat.value & SDL_HAT_RIGHT) != 0);
    }
  }

  hat_state = jhat.value;
}

void
JoystickManager::process_axis_event(const SDL_JoyAxisEvent& jaxis)
{
  if (wait_for_joystick >= 0)
  {
    if (abs(jaxis.value) > m_joystick_config.m_dead_zone) {
      if (jaxis.value < 0)
        m_joystick_config.bind_joyaxis(jaxis.which, -(jaxis.axis + 1), Control(wait_for_joystick));
      else
        m_joystick_config.bind_joyaxis(jaxis.which, jaxis.axis + 1, Control(wait_for_joystick));

      MenuManager::instance().refresh();
      wait_for_joystick = -1;
    }
  }
  else
  {
    // Split the axis into left and right, so that both can be
    // mapped separately (needed for jump/down vs up/down)
    int axis = jaxis.axis + 1;

    auto left = m_joystick_config.m_joy_axis_map.find(std::make_pair(jaxis.which, -axis));
    auto right = m_joystick_config.m_joy_axis_map.find(std::make_pair(jaxis.which, axis));

    if (left == m_joystick_config.m_joy_axis_map.end()) {
      // std::cout << "Unmapped joyaxis " << (int)jaxis.axis << " moved" << std::endl;
    } else {
      if (jaxis.value < -m_joystick_config.m_dead_zone)
        set_joy_controls(jaxis.which, left->second,  true);
      else
        set_joy_controls(jaxis.which, left->second, false);
    }

    if (right == m_joystick_config.m_joy_axis_map.end()) {
      // std::cout << "Unmapped joyaxis " << (int)jaxis.axis << " moved" << std::endl;
    } else {
      if (jaxis.value > m_joystick_config.m_dead_zone)
        set_joy_controls(jaxis.which, right->second, true);
      else
        set_joy_controls(jaxis.which, right->second, false);
    }
  }
}

void
JoystickManager::process_button_event(const SDL_JoyButtonEvent& jbutton)
{
  if (wait_for_joystick >= 0)
  {
    if (jbutton.state == SDL_PRESSED)
    {
      m_joystick_config.bind_joybutton(jbutton.which, jbutton.button, static_cast<Control>(wait_for_joystick));
      MenuManager::instance().refresh();
      parent->reset();
      wait_for_joystick = -1;
    }
  }
  else
  {
    auto i = m_joystick_config.m_joy_button_map.find(std::make_pair(jbutton.which, jbutton.button));
    if (i == m_joystick_config.m_joy_button_map.end()) {
      log_debug << "Unmapped joybutton " << static_cast<int>(jbutton.button) << " pressed" << std::endl;
    } else {
      set_joy_controls(jbutton.which, i->second, (jbutton.state == SDL_PRESSED));
    }
  }
}

void
JoystickManager::bind_next_event_to(Control id)
{
  wait_for_joystick = static_cast<int>(id);
}

void
JoystickManager::set_joy_controls(SDL_JoystickID joystick, Control id, bool value)
{
  auto it = joysticks.find(SDL_JoystickFromInstanceID(joystick));
  if (it == joysticks.end() || it->second < 0)
    return;

  if (m_joystick_config.m_jump_with_up_joy &&
      id == Control::UP)
  {
    parent->get_controller(it->second).set_control(Control::JUMP, value);
  }

  parent->get_controller(it->second).set_control(id, value);
}

void
JoystickManager::on_player_removed(int player_id)
{
  auto it2 = std::find_if(joysticks.begin(), joysticks.end(), [player_id](decltype(joysticks)::const_reference pair) {
    return pair.second == player_id;
  });
  if (it2 != joysticks.end())
  {
    it2->second = -1;
    // Try again, in case multiple controllers were bount to a player
    // Recursive call shouldn't go too deep except in hardcore scenarios
    on_player_removed(player_id);
  }
}

bool
JoystickManager::has_corresponding_joystick(int player_id) const
{
  return std::find_if(joysticks.begin(), joysticks.end(), [player_id](decltype(joysticks)::const_reference pair) {
    return pair.second == player_id;
  }) != joysticks.end();
}

int
JoystickManager::rumble(SDL_Joystick* controller) const
{
#if SDL_VERSION_ATLEAST(2, 0, 9)
  if (g_config->multiplayer_buzz_controllers)
  {
#if SDL_VERSION_ATLEAST(2, 0, 18)
    if (SDL_JoystickHasRumble(controller))
    {
#endif
      // TODO: Rumble intensity setting (like volume)
      SDL_JoystickRumble(controller, 0xFFFF, 0xFFFF, 300);
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
JoystickManager::bind_joystick(SDL_Joystick* controller, int player_id)
{
  joysticks[controller] = player_id;

  if (!g_config->multiplayer_multibind)
    for (auto& pair2 : joysticks)
      if (pair2.second == player_id && pair2.first != controller)
        pair2.second = -1;
}


/* EOF */
