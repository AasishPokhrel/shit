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

#ifndef HEADER_SUPERTUX_INTERFACE_LABEL_HPP
#define HEADER_SUPERTUX_INTERFACE_LABEL_HPP

#include <SDL.h>

#include "editor/widget.hpp"
#include "video/drawing_context.hpp"

class InterfaceLabel : public Widget
{
public:
  InterfaceLabel();
  InterfaceLabel(const Rectf& rect, std::string label);
  ~InterfaceLabel() override {}

  virtual void draw(DrawingContext& context) override;
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;

  inline void set_rect(const Rectf& rect) { m_rect = rect; }
  inline Rectf get_rect() const { return m_rect; }

  inline void set_label(const std::string& label) { m_label = label; }
  inline const std::string& get_label() const { return m_label; }

  bool fits(const std::string& text) const;
  std::string get_truncated_text() const;

protected:
  /** The rectangle where the InterfaceLabel should be rendered */
  Rectf m_rect;
  /** The text of the label */
  std::string m_label;

private:
  Vector m_mouse_pos;

private:
  InterfaceLabel(const InterfaceLabel&) = delete;
  InterfaceLabel& operator=(const InterfaceLabel&) = delete;
};

#endif

/* EOF */
