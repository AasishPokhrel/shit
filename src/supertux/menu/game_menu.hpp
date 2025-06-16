//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_GAME_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_GAME_MENU_HPP

#include "gui/menu.hpp"

#include <functional>

class GameMenu final : public Menu
{
private:
  // stores callback for level reset
  std::function<void ()> reset_callback;
  // stores callback for level reset from checkpoint
  std::function<void ()> reset_checkpoint_callback;
  // stores callback for level abort
  std::function<void ()> abort_callback;

public:
  GameMenu();

  void menu_action(MenuItem& item) override;

private:
  enum GameMenuIDs {
    MNID_CONTINUE,
    MNID_RESETLEVEL,
    MNID_RESETLEVELCHECKPOINT,
    MNID_ABORTLEVEL
  };

private:
  GameMenu(const GameMenu&) = delete;
  GameMenu& operator=(const GameMenu&) = delete;
};

#endif

/* EOF */
