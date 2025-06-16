//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "editor/widget.hpp"

bool
Widget::event(const SDL_Event& ev)
{
  switch (ev.type)
  {
    case SDL_MOUSEBUTTONDOWN:
      return on_mouse_button_down(ev.button);

    case SDL_MOUSEBUTTONUP:
      return on_mouse_button_up(ev.button);

    case SDL_MOUSEMOTION:
      return on_mouse_motion(ev.motion);

    case SDL_MOUSEWHEEL:
      return on_mouse_wheel(ev.wheel);

    case SDL_KEYDOWN:
      return on_key_down(ev.key);

    case SDL_KEYUP:
      return on_key_up(ev.key);

    default:
      return false;
  }
}

/* EOF */
