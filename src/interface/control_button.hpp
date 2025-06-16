//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_BUTTON_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_BUTTON_HPP

#include "interface/control.hpp"

class ControlButton : public InterfaceControl
{
public:
  ControlButton(std::string label);

  virtual void draw(DrawingContext& context) override;
  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  virtual bool on_key_up(const SDL_KeyboardEvent& key) override;
  virtual bool on_key_down(const SDL_KeyboardEvent& key) override;

private:
  std::string m_btn_label;
  bool m_mouse_down;

private:
  ControlButton(const ControlButton&) = delete;
  ControlButton& operator=(const ControlButton&) = delete;
};

#endif

/* EOF */
