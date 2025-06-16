//  SuperTux
//  Copyright (C) 2022 Vankata453
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

#include "gui/notification.hpp"

#include "control/controller.hpp"
#include "gui/menu_manager.hpp"
#include "gui/mousecursor.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "video/color.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"

Notification::Notification(std::string id, bool no_auto_hide, bool no_auto_disable) :
  m_id(id),
  m_auto_hide(!no_auto_hide),
  m_auto_disable(!no_auto_disable),
  m_text(),
  m_mini_text(),
  m_text_size(),
  m_mini_text_size(),
  m_pos(),
  m_size(),
  m_mouse_pos(),
  m_mouse_over(false),
  m_mouse_over_sym1(false),
  m_mouse_over_sym2(false),
  m_quit(false),
  m_callback([](){})
{
  if (is_disabled(id)) // The notification exists in the config as disabled.
  {
    log_warning << "Requested launch of disabled notification with ID \"" << m_id << "\". Closing." << std::endl;
    m_quit = true;
    return;
  }

  set_mini_text(_("Click for more details.")); // Set default mini text.
}

Notification::~Notification()
{
}

void
Notification::set_text(const std::string& text)
{
  m_text = text;

  m_text_size = Sizef(Resources::normal_font->get_text_width(m_text),
                      Resources::normal_font->get_text_height(m_text));
  calculate_size();
}

void
Notification::set_mini_text(const std::string& text)
{
  m_mini_text = text;

  m_mini_text_size = Sizef(Resources::small_font->get_text_width(m_mini_text),
                           Resources::small_font->get_text_height(m_mini_text));
  calculate_size();
}

void
Notification::calculate_size()
{
  m_size = Sizef(std::max(m_text_size.width, m_mini_text_size.width) + 60.0f,
                 m_text_size.height + m_mini_text_size.height + 40.0f);
}

void
Notification::draw(DrawingContext& context)
{
  if (m_quit || !MenuManager::instance().is_active()) // Close notification, if a quit has been requested, or the MenuManager isn't active.
  {
    close();
    return;
  }

  m_pos = Vector(context.get_width() - std::max(m_text_size.width, m_mini_text_size.width) - 90.0f,
                 static_cast<float>(context.get_height() / 12) - m_text_size.height - m_mini_text_size.height + 10.0f);
  Rectf bg_rect(m_pos, m_size);

  // Draw background rect
  context.color().draw_filled_rect(bg_rect.grown(12.0f),
                                     Color(g_config->menubackcolor.red, g_config->menubackcolor.green,
                                       g_config->menubackcolor.blue, std::max(0.f, g_config->menubackcolor.alpha - 0.5f)),
                                       g_config->menuroundness + 4.f,
                                     LAYER_GUI - 10);

  context.color().draw_filled_rect(bg_rect.grown(8.0f),
                                     Color(g_config->menufrontcolor.red, g_config->menufrontcolor.green,
                                       g_config->menufrontcolor.blue, std::max(0.f, g_config->menufrontcolor.alpha - 0.3f)),
                                       g_config->menuroundness,
                                     LAYER_GUI - 10);

  // Draw normal and mini texts.
  context.color().draw_text(Resources::normal_font, m_text,
                              Vector(bg_rect.get_left() + bg_rect.get_width() / 2.0f,
                                     bg_rect.get_top() + (bg_rect.get_height() / 2 - bg_rect.get_height() / 3)),
                              ALIGN_CENTER, LAYER_GUI);

  context.color().draw_text(Resources::small_font, m_mini_text,
                              Vector(bg_rect.get_left() + bg_rect.get_width() / 2.0f,
                                     bg_rect.get_top() + (bg_rect.get_height() / 2 + bg_rect.get_height() / 8)),
                              ALIGN_CENTER, LAYER_GUI);

  // Draw "Do not show again" and "Close" symbols, if the mouse is hovering over the notification.
  if (!m_mouse_over) return;
  const std::string sym1 = "-";
  const std::string sym2 = "X";
  Vector sym1_pos = Vector(bg_rect.get_left() + 5.0f, bg_rect.get_top());
  Vector sym2_pos = Vector(bg_rect.get_right() - 15.0f, bg_rect.get_top());

  context.color().draw_text(Resources::normal_font, sym1, sym1_pos,
                              ALIGN_LEFT, LAYER_GUI, Color::YELLOW);

  context.color().draw_text(Resources::normal_font, sym2, sym2_pos,
                              ALIGN_LEFT, LAYER_GUI, Color::RED);

  // Draw description of a symbol, when the mouse hovers over it.
  Rectf sym1_rect(sym1_pos, Sizef(Resources::normal_font->get_text_width(sym1), Resources::normal_font->get_text_height(sym1)));
  Rectf sym2_rect(sym2_pos, Sizef(Resources::normal_font->get_text_width(sym2), Resources::normal_font->get_text_height(sym2)));

  m_mouse_over_sym1 = sym1_rect.contains(m_mouse_pos);
  m_mouse_over_sym2 = sym2_rect.contains(m_mouse_pos);

  if (m_mouse_over_sym1)
  {
    context.color().draw_text(Resources::normal_font, _("Do not show again"),
                                Vector(m_mouse_pos.x,
                                       m_mouse_pos.y + 20.0f),
                                ALIGN_RIGHT, LAYER_GUI + 1, Color::CYAN);
  }
  else if (m_mouse_over_sym2)
  {
    context.color().draw_text(Resources::normal_font, _("Close"),
                                Vector(m_mouse_pos.x,
                                       m_mouse_pos.y + 20.0f),
                                ALIGN_RIGHT, LAYER_GUI + 1, Color::CYAN);
  }
}

void
Notification::event(const SDL_Event& ev)
{
  Rectf bg_rect(m_pos, m_size);
  bg_rect = bg_rect.grown(12.0f);

  switch (ev.type)
  {
    case SDL_MOUSEBUTTONDOWN:
    if (ev.button.button == SDL_BUTTON_LEFT)
    {
      if (m_mouse_over)
      {
        if (m_mouse_over_sym1) // "Do not show again" action
        {
          disable();
          close();
        }
        else if (m_mouse_over_sym2) // "Close" action
        {
          close();
        }
        else // Notification clicked (execute callback)
        {
          m_callback();
          if (m_auto_disable) disable();
          if (m_auto_hide) close();
        }
      }
    }
    break;

    case SDL_MOUSEMOTION:
    {
      m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(ev.motion.x, ev.motion.y);
      m_mouse_over = bg_rect.contains(m_mouse_pos);
      if (MouseCursor::current() && m_mouse_over) MouseCursor::current()->set_state(MouseCursorState::LINK);
    }
    break;

    default:
      break;
  }
}

void
Notification::process_input(const Controller& controller)
{
  if (controller.pressed(Control::MENU_BACK) && !MenuManager::instance().previous_menu())
  {
    // If the user tries to go back on the last menu, close the notification instead.
    close();
  }
}

// Notification actions

void
Notification::disable()
{
  // Save the notification as disabled in the config.
  bool defined = false;
  for (auto& notif : g_config->notifications)
  {
    if (notif.id == m_id)
    {
      // If the notification is already defined in the config, only set "disabled" to "true".
      defined = true;
      notif.disabled = true;
    }
  }
  if (!defined) g_config->notifications.push_back({m_id, true}); 
}

void
Notification::close()
{
  if (MouseCursor::current() && m_mouse_over) MouseCursor::current()->set_state(MouseCursorState::NORMAL);
  MenuManager::instance().set_notification({});
}

// Static functions, serving as utilities

bool
Notification::is_disabled(std::string id) // Check if a notification is disabled by its ID.
{
  return std::any_of(g_config->notifications.begin(), g_config->notifications.end(),
                     [id](const auto& notif)
                     {
                       return notif.id == id && notif.disabled;
                     });
}

/* EOF */
