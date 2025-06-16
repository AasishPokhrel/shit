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

#include "interface/label.hpp"

#include "supertux/resources.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

InterfaceLabel::InterfaceLabel() :
  m_rect(),
  m_label(),
  m_mouse_pos(0.0f, 0.0f)
{
}

InterfaceLabel::InterfaceLabel(const Rectf& rect, std::string label) :
  m_rect(rect),
  m_label(std::move(label)),
  m_mouse_pos(0.0f, 0.0f)
{
}

bool
InterfaceLabel::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  return false;
}

void
InterfaceLabel::draw(DrawingContext& context)
{
  context.color().draw_text(Resources::control_font,
                            get_truncated_text(),
                            Vector(m_rect.get_left() + 5.f,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 -
                                    Resources::control_font->get_height() / 2 + 1.f),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI,
                            Color::WHITE);

  if (!fits(m_label) && m_rect.contains(m_mouse_pos)) {
    context.color().draw_filled_rect(Rectf(m_mouse_pos, m_mouse_pos + Vector(
                                       Resources::control_font
                                                    ->get_text_width(m_label),
                                       Resources::control_font->get_height()))
                                       .grown(5.f).moved(Vector(0, 32)),
                                     Color(0.1f, 0.1f, 0.1f, 0.8f),
                                     LAYER_GUI + 10);
    context.color().draw_filled_rect(Rectf(m_mouse_pos, m_mouse_pos + Vector(
                                       Resources::control_font
                                                    ->get_text_width(m_label),
                                       Resources::control_font->get_height()))
                                       .grown(3.f).moved(Vector(0, 32)),
                                     Color(1.f, 1.f, 1.f, 0.1f),
                                     LAYER_GUI + 10);
    context.color().draw_text(Resources::control_font,
                              m_label,
                              m_mouse_pos + Vector(0, 33.f),
                              FontAlignment::ALIGN_LEFT,
                              LAYER_GUI + 11,
                              Color::WHITE);
  }
}

bool
InterfaceLabel::fits(const std::string& text) const
{
  return Resources::control_font->get_text_width(text) <= m_rect.get_width();
}

std::string
InterfaceLabel::get_truncated_text() const
{
  if (fits(m_label)) return m_label;

  std::string temp = m_label;
  while (!temp.empty() && !fits(temp + "..."))
    temp.pop_back();

  return temp + "...";
}


