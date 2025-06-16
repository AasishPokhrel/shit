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

#include "gui/dialog.hpp"

#include <algorithm>

#include "control/controller.hpp"
#include "gui/mousecursor.hpp"
#include "math/util.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

Dialog::Dialog(bool passive, bool auto_clear_dialogs) :
  m_text(),
  m_buttons(),
  m_selected_button(),
  m_cancel_button(-1),
  m_passive(passive),
  m_clear_diags(auto_clear_dialogs),
  m_text_size(),
  m_size()
{
}

Dialog::~Dialog()
{
}

void
Dialog::set_text(const std::string& text)
{
  m_text = text;

  m_text_size = Sizef(Resources::normal_font->get_text_width(m_text),
                      Resources::normal_font->get_text_height(m_text));
  m_size = Sizef(m_text_size.width,
                m_text_size.height + 44);
}

void
Dialog::clear_buttons()
{
  m_buttons.clear();
  m_selected_button = 0;
  m_cancel_button = -1;
}

void
Dialog::add_default_button(const std::string& text, const std::function<void ()>& callback)
{
  add_button(text, callback);
  m_selected_button = static_cast<int>(m_buttons.size()) - 1;
}

void
Dialog::add_cancel_button(const std::string& text, const std::function<void ()>& callback)
{
  add_button(text, callback);
  m_cancel_button = static_cast<int>(m_buttons.size() - 1);
}

void
Dialog::add_button(const std::string& text, const std::function<void ()>& callback)
{
  m_buttons.push_back({text, callback});
}

int
Dialog::get_button_at(const Vector& mouse_pos) const
{
  Rectf bg_rect(Vector(static_cast<float>(SCREEN_WIDTH) / 2.0f - m_text_size.width / 2.0f,
                       static_cast<float>(SCREEN_HEIGHT) / 2.0f - m_text_size.height / 2.0f),
                m_size);

  for (int i = 0; i < static_cast<int>(m_buttons.size()); ++i)
  {
    float segment_width = bg_rect.get_width() / static_cast<float>(m_buttons.size());
    float button_width = segment_width;
    float button_height = 24.0f;
    Vector pos(bg_rect.get_left() + segment_width/2.0f + static_cast<float>(i) * segment_width,
               bg_rect.get_bottom() - 12);
    Rectf button_rect(Vector(pos.x - button_width/2, pos.y - button_height/2),
                      Vector(pos.x + button_width/2, pos.y + button_height/2));
    if (button_rect.contains(mouse_pos))
    {
      return i;
    }
  }
  return -1;
}

void
Dialog::event(const SDL_Event& ev)
{
  if (m_passive) // Passive dialogs don't accept events.
    return;

  switch (ev.type) {
    case SDL_MOUSEBUTTONDOWN:
    if (ev.button.button == SDL_BUTTON_LEFT)
    {
      Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(ev.motion.x, ev.motion.y);
      int new_button = get_button_at(mouse_pos);
      if (new_button != -1)
      {
        m_selected_button = new_button;
        on_button_click(m_selected_button);
      }
    }
    break;

    case SDL_MOUSEMOTION:
    {
      Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(ev.motion.x, ev.motion.y);
      int new_button = get_button_at(mouse_pos);
      if (new_button != -1)
      {
        m_selected_button = new_button;
        if (MouseCursor::current())
          MouseCursor::current()->set_state(MouseCursorState::LINK);
      }
      else
      {
        if (MouseCursor::current())
          MouseCursor::current()->set_state(MouseCursorState::NORMAL);
      }
    }
    break;

    default:
      break;
  }
}

void
Dialog::process_input(const Controller& controller)
{
  if (m_passive) // Passive dialogs don't accept events.
    return;

  if (controller.pressed(Control::LEFT))
  {
    m_selected_button -= 1;
    m_selected_button = std::max(m_selected_button, 0);
  }

  if (controller.pressed(Control::RIGHT))
  {
    m_selected_button += 1;
    m_selected_button = std::min(m_selected_button, static_cast<int>(m_buttons.size()) - 1);
  }

  if (controller.pressed(Control::ACTION) ||
      controller.pressed(Control::JUMP) ||
      controller.pressed(Control::MENU_SELECT) ||
      controller.pressed(Control::MENU_SELECT_SPACE))
  {
    on_button_click(m_selected_button);
  }

  if (m_cancel_button != -1 &&
      (controller.pressed(Control::ESCAPE) ||
       controller.pressed(Control::MENU_BACK)))
  {
    on_button_click(m_cancel_button);
  }
}

void
Dialog::draw(DrawingContext& context)
{
  Rectf bg_rect(Vector(static_cast<float>(m_passive ?
                                          (context.get_width() - m_text_size.width - 20.0f) :
                                          context.get_width() / 2.0f - m_text_size.width / 2.0f),
                       static_cast<float>(m_passive ?
                                          (context.get_height() - m_text_size.height - 65.0f) :
                                          (context.get_height() / 2.0f - m_text_size.height / 2.0f))),
                m_size);

  // Draw background rect.
  context.color().draw_filled_rect(bg_rect.grown(12.0f),
                                     Color(g_config->menubackcolor.red, g_config->menubackcolor.green,
                                       g_config->menubackcolor.blue, (std::max(0.f, g_config->menubackcolor.alpha - (m_passive ? 0.5f : 0.0f)))),
                                       g_config->menuroundness + 4.f,
                                     LAYER_GUI-10);

  context.color().draw_filled_rect(bg_rect.grown(8.0f),
                                     Color(g_config->menufrontcolor.red, g_config->menufrontcolor.green,
                                       g_config->menufrontcolor.blue, (std::max(0.f, g_config->menufrontcolor.alpha - (m_passive ? 0.3f : 0.0f)))),
                                       g_config->menuroundness,
                                     LAYER_GUI-10);

  // Draw text.
  context.color().draw_text(Resources::normal_font, m_text,
                              Vector(bg_rect.get_left() + bg_rect.get_width()/2.0f,
                                     bg_rect.get_top()),
                              ALIGN_CENTER, LAYER_GUI);
  if (m_passive)
    return;

  // Draw horizontal line.
  context.color().draw_filled_rect(Rectf(Vector(bg_rect.get_left(), bg_rect.get_bottom() - 35),
                                         Sizef(bg_rect.get_width(), 4)),
                                   g_config->hlcolor, LAYER_GUI);
  context.color().draw_filled_rect(Rectf(Vector(bg_rect.get_left(), bg_rect.get_bottom() - 35),
                                         Sizef(bg_rect.get_width(), 2)),
                                   Color(1.0f, 1.0f, 1.0f, 1.0f), LAYER_GUI);

  // Draw buttons.
  for (int i = 0; i < static_cast<int>(m_buttons.size()); ++i)
  {
    float segment_width = bg_rect.get_width() / static_cast<float>(m_buttons.size());
    Vector pos(bg_rect.get_left() + segment_width/2.0f + static_cast<float>(i) * segment_width,
               bg_rect.get_bottom() - 12);

    if (i == m_selected_button)
    {
      float button_width = segment_width;
      float button_height = 24.0f;
      float blink = (sinf(g_real_time * math::PI * 1.0f)/2.0f + 0.5f) * 0.5f + 0.25f;
      context.color().draw_filled_rect(Rectf(Vector(pos.x - button_width/2, pos.y - button_height/2),
                                               Vector(pos.x + button_width/2, pos.y + button_height/2)).grown(2.0f),
                                         Color(1.0f, 1.0f, 1.0f, blink),
                                         g_config->menuroundness * 0.825f,
                                         LAYER_GUI-10);
      context.color().draw_filled_rect(Rectf(Vector(pos.x - button_width/2, pos.y - button_height/2),
                                               Vector(pos.x + button_width/2, pos.y + button_height/2)),
                                         Color(1.0f, 1.0f, 1.0f, 0.5f),
                                         g_config->menuroundness * 0.75f,
                                         LAYER_GUI-10);
    }

    context.color().draw_text(Resources::normal_font, m_buttons[i].text,
                              Vector(pos.x, pos.y - static_cast<float>(int(Resources::normal_font->get_height() / 2))),
                              ALIGN_CENTER, LAYER_GUI,
                              i == m_selected_button ? g_config->activetextcolor : ColorScheme::Menu::default_color);
  }
}

Vector
Dialog::get_center_pos() const
{
  return Vector(static_cast<float>(SCREEN_WIDTH) / 2.0f,
               static_cast<float>(SCREEN_HEIGHT) / 2.0f + m_text_size.height / 2.0f);
}

void
Dialog::on_button_click(int button) const
{
  if (m_buttons[button].callback)
  {
    m_buttons[button].callback();
  }
  if (m_clear_diags || button == m_cancel_button)
  {
    MenuManager::instance().set_dialog({});
  }
}

/* EOF */
