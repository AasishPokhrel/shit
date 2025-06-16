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

#ifndef HEADER_SUPERTUX_EDITOR_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_WIDGET_HPP

#include <SDL.h>

class DrawingContext;

class Widget
{
private:
public:
  Widget() {}
  virtual ~Widget() {}

  virtual bool event(const SDL_Event& ev);
  virtual void draw(DrawingContext& context) {}
  virtual void update(float dt_sec) {}

  virtual void setup() {}
  virtual void on_window_resize() {}

  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) { return false; }
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) { return false; }
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) { return false; }
  virtual bool on_mouse_wheel(const SDL_MouseWheelEvent& wheel) { return false; }
  virtual bool on_key_up(const SDL_KeyboardEvent& key) { return false; }
  virtual bool on_key_down(const SDL_KeyboardEvent& key) { return false; }

private:
  Widget(const Widget&) = delete;
  Widget& operator=(const Widget&) = delete;
};

#endif

/* EOF */
