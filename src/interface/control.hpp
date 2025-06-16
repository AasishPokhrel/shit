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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_HPP

#include <functional>
#include <SDL.h>

#include "control/input_manager.hpp"
#include "editor/widget.hpp"
#include "interface/label.hpp"
#include "video/drawing_context.hpp"

class InterfaceControl : public Widget
{
public:
  InterfaceControl();
  ~InterfaceControl() override {}

  virtual void draw(DrawingContext& context) override { if (m_label) m_label->draw(context); }
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override { if (m_label) m_label->on_mouse_motion(motion); return false; }

  inline void set_focus(bool focus) { m_has_focus = focus; }
  inline bool has_focus() const { return m_has_focus; }

  inline void set_rect(const Rectf& rect) { m_rect = rect; }
  inline Rectf get_rect() const { return m_rect; }

public:
  /** Optional; a function that will be called each time the bound value
   *  is modified.
   */
  std::function<void()> m_on_change;

  /** Optional; the label associated with the control */
  std::unique_ptr<InterfaceLabel> m_label;

protected:
  /** Whether or not the user has this InterfaceControl as focused */
  bool m_has_focus;
  /** The rectangle where the InterfaceControl should be rendered */
  Rectf m_rect;
  /** A pointer to the parent container, or null if not in any container. */
  InterfaceControl* m_parent;

private:
  InterfaceControl(const InterfaceControl&) = delete;
  InterfaceControl& operator=(const InterfaceControl&) = delete;
};

#endif

/* EOF */
