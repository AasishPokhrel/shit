//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "gui/mousecursor.hpp"

#include <SDL.h>

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "sprite/sprite.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

MouseCursor* MouseCursor::current_ = nullptr;

MouseCursor::MouseCursor(SpritePtr sprite) :
  m_state(MouseCursorState::NORMAL),
  m_applied_state(MouseCursorState::HIDE),
  m_sprite(std::move(sprite)),
  m_x(),
  m_y(),
  m_mobile_mode(false),
  m_icon()
{
}

void
MouseCursor::apply_state(MouseCursorState state)
{
  if (m_applied_state != state)
  {
    m_applied_state = state;

    switch(state)
    {
      case MouseCursorState::NORMAL:
        m_sprite->set_action("default");
        break;

      case MouseCursorState::CLICK:
        m_sprite->set_action("click");
        break;

      case MouseCursorState::LINK:
        m_sprite->set_action("link");
        break;

      case MouseCursorState::HIDE:
        break;
    }
  }
}

void
MouseCursor::draw(DrawingContext& context)
{
  if (!g_config->custom_mouse_cursor) return;
  if (m_state != MouseCursorState::HIDE)
  {
    int x, y;
    Uint32 ispressed = SDL_GetMouseState(&x, &y);

    if (m_mobile_mode)
    {
      x = m_x;
      y = m_y;
    }

    if (ispressed & SDL_BUTTON(1) || ispressed & SDL_BUTTON(2))
    {
      apply_state(MouseCursorState::CLICK);
    }
    else
    {
      apply_state(m_state);
    }

    Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(x, y);

    m_sprite->draw(context.color(), mouse_pos, LAYER_GUI + 100);

    if (m_icon) {
      context.color().draw_surface(m_icon,
                                   Vector(mouse_pos.x + static_cast<float>(m_icon->get_width()) / 2,
                                          mouse_pos.y - static_cast<float>(m_icon->get_height())),
                                   LAYER_GUI + 100);
    }
  }
}

/* EOF */
