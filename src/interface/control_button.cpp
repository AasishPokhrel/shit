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

#include "interface/control_button.hpp"

#include "math/vector.hpp"
#include "supertux/resources.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

ControlButton::ControlButton(std::string label) :
  m_btn_label(std::move(label)),
  m_mouse_down(false)
{
}

void
ControlButton::draw(DrawingContext& context)
{
  InterfaceControl::draw(context);

  context.color().draw_filled_rect(m_rect,
                                   m_mouse_down ? Color(0.3f, 0.3f, 0.3f, 1.f) :
                                       (m_has_focus ? Color(0.75f, 0.75f, 0.7f, 1.f) : Color(0.5f, 0.5f, 0.5f, 1.f)),
                                   LAYER_GUI);

  context.color().draw_text(Resources::control_font,
                            m_btn_label,
                            Vector((m_rect.get_left() + m_rect.get_right()) / 2,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 - Resources::control_font->get_height() / 2),
                            FontAlignment::ALIGN_CENTER,
                            LAYER_GUI,
                            Color::BLACK);
}

bool
ControlButton::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  if (button.button != SDL_BUTTON_LEFT)
    return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  if (!m_rect.contains(mouse_pos) || !m_mouse_down) {
    m_mouse_down = false;
    return false;
  }

  m_mouse_down = false;

  if (m_on_change)
    m_on_change();

  m_has_focus = true;

  return true;
}

bool
ControlButton::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (button.button == SDL_BUTTON_LEFT) {
    Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
    if (!m_rect.contains(mouse_pos)) {
      m_has_focus = false;
    } else {
      m_has_focus = true;
      m_mouse_down = true;
    }
  }
  return false;
}

bool
ControlButton::on_key_up(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  if (key.keysym.sym == SDLK_SPACE) {
    if (m_on_change)
      m_on_change();
    m_mouse_down = false;
    return true;
  }

  return false;
}

bool
ControlButton::on_key_down(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  if (key.keysym.sym == SDLK_SPACE) {
    m_mouse_down = true;
    return true;
  }

  return false;
}

/* EOF */
