//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//                2023 Vankata453
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

#include "gui/menu_transition.hpp"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"

MenuTransition::MenuTransition() :
  m_from_rect(),
  m_to_rect(),
  m_effect_progress(1.0f),
  m_effect_start_time(),
  m_is_active(false)
{
}

void
MenuTransition::start(const Rectf& from_rect, const Rectf& to_rect)
{
  m_from_rect = from_rect;
  m_to_rect = to_rect;

  m_effect_start_time = g_real_time;
  m_effect_progress = 0.0f;

  m_is_active = true;
}

void
MenuTransition::update()
{
  if (!g_config->transitions_enabled && m_is_active)
  {
    m_effect_progress = 1.0f;
    m_is_active = false;
    return;
  }
  if (m_is_active)
  {
    m_effect_progress = (g_real_time - m_effect_start_time) * 6.0f;

    if (m_effect_progress > 1.0f)
    {
      m_effect_progress = 1.0f;
      m_is_active = false;
    }
  }
}

void
MenuTransition::draw(DrawingContext& context)
{
  const float& p = m_effect_progress;

  Rectf rect = m_to_rect;
  if (m_is_active)
  {
    rect = Rectf((m_to_rect.get_left() * p) + (m_from_rect.get_left() * (1.0f - p)),
                 (m_to_rect.get_top() * p) + (m_from_rect.get_top() * (1.0f - p)),
                 (m_to_rect.get_right() * p) + (m_from_rect.get_right() * (1.0f - p)),
                 (m_to_rect.get_bottom() * p) + (m_from_rect.get_bottom() * (1.0f - p)));
  }

  // draw menu background rectangles
  context.color().draw_filled_rect(Rectf(rect.get_left() - 4, rect.get_top() - 10-4,
                                         rect.get_right() + 4, rect.get_bottom() + 10 + 4),
                                     g_config->menubackcolor,
                                     g_config->menuroundness + 4.f,
                                     LAYER_GUI-10);

  context.color().draw_filled_rect(Rectf(rect.get_left(), rect.get_top() - 10,
                                         rect.get_right(), rect.get_bottom() + 10),
                                     g_config->menufrontcolor,
                                     g_config->menuroundness,
                                     LAYER_GUI-10);
}

/* EOF */
