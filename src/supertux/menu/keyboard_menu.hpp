//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
//                2007 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_KEYBOARD_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_KEYBOARD_MENU_HPP

#include "control/controller.hpp"
#include "gui/menu_item.hpp"

class InputManager;

class KeyboardMenu final : public Menu
{
public:
  KeyboardMenu(InputManager& input_manager, int player_id = 0);

  void refresh() override;
  std::string get_key_name(SDL_Keycode key) const;
  void menu_action(MenuItem& item) override;

private:
  void refresh_control(const Control& control);

private:
  InputManager& m_input_manager;
  int m_player_id;

private:
  KeyboardMenu(const KeyboardMenu&) = delete;
  KeyboardMenu& operator=(const KeyboardMenu&) = delete;
};

#endif

/* EOF */
