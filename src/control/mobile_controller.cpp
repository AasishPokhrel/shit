//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

#include "control/mobile_controller.hpp"


#include <string>

#include "SDL.h"

#include "control/controller.hpp"
#include "math/vector.hpp"
#include "supertux/globals.hpp"
#include "supertux/gameconfig.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

MobileController::MobileController() :
  m_up(false),
  m_down(false),
  m_left(false),
  m_right(false),
  m_jump(false),
  m_action(false),
  m_cheats(false),
  m_debug(false),
  m_escape(false),
  m_old_up(false),
  m_old_down(false),
  m_old_left(false),
  m_old_right(false),
  m_old_jump(false),
  m_old_action(false),
  m_old_cheats(false),
  m_old_debug(false),
  m_old_escape(false),
  m_fingers(),
  m_rect_directions(16.f, -144.f, 144.f, -16.f),
  m_rect_jump(-160.f, -80.f, -96.f, -16.f),
  m_rect_action(-80.f, -80.f, -16.f, -16.f),
  m_rect_cheats(-160.f, 16.f, -96.f, 80.f),
  m_rect_debug(-80.f, 16.f, -16.f, 80.f),
  m_rect_escape(16.f, 16.f, 64.f, 64.f),
  m_draw_directions(16.f, -144.f, 144.f, -16.f),
  m_draw_jump(-160.f, -80.f, -96.f, -16.f),
  m_draw_action(-80.f, -80.f, -16.f, -16.f),
  m_draw_cheats(-160.f, 16.f, -96.f, 80.f),
  m_draw_debug(-80.f, 16.f, -16.f, 80.f),
  m_draw_escape(16.f, 16.f, 64.f, 64.f),
  m_tex_dirs(Surface::from_file("/images/engine/mobile/direction.png")),
  m_tex_btn(Surface::from_file("/images/engine/mobile/button.png")),
  m_tex_btn_press(Surface::from_file("/images/engine/mobile/button_press.png")),
  m_tex_pause(Surface::from_file("/images/engine/mobile/pause.png")),
  m_tex_up(Surface::from_file("/images/engine/mobile/direction_hightlight_up.png")),
  m_tex_dwn(Surface::from_file("/images/engine/mobile/direction_hightlight_down.png")),
  m_tex_lft(Surface::from_file("/images/engine/mobile/direction_hightlight_left.png")),
  m_tex_rgt(Surface::from_file("/images/engine/mobile/direction_hightlight_right.png")),
  m_tex_jump(Surface::from_file("/images/engine/mobile/jump.png")),
  m_tex_action(Surface::from_file("/images/engine/mobile/action.png")),
  m_tex_cheats(Surface::from_file("/images/engine/mobile/cheats.png")),
  m_tex_debug(Surface::from_file("/images/engine/mobile/debug.png")),
  m_screen_width(),
  m_screen_height(),
  m_mobile_controls_scale()
{
}

void
MobileController::draw(DrawingContext& context)
{
  if (!g_config->mobile_controls)
    return;

  if (m_screen_width != static_cast<int>(context.get_width()) ||
      m_screen_height != static_cast<int>(context.get_height()) ||
      m_mobile_controls_scale != g_config->m_mobile_controls_scale)
  {
    m_screen_width = static_cast<int>(context.get_width());
    m_screen_height = static_cast<int>(context.get_height());
    float width = static_cast<float>(m_screen_width);
    float height = static_cast<float>(m_screen_height);
    m_mobile_controls_scale = g_config->m_mobile_controls_scale;
    // Buttons on Android are bigger, and direction buttons are extra wide
    // Use screen height to calculate button size, because 20:9 screen ratios are common
#ifdef __ANDROID__
    const float BUTTON_SCALE = 0.4f * g_config->m_mobile_controls_scale;
#else
    const float BUTTON_SCALE = 0.2f * g_config->m_mobile_controls_scale;
#endif
    m_rect_directions.set_size(height * BUTTON_SCALE * 4 / 3, height * BUTTON_SCALE);
    m_rect_directions.set_pos(Vector(0, height - height * BUTTON_SCALE));
    m_draw_directions = Rectf::from_center(m_rect_directions.get_middle(),
      Sizef(m_rect_directions.get_height() / 2, m_rect_directions.get_height() / 2));

    m_rect_jump.set_size(height * BUTTON_SCALE, height * BUTTON_SCALE);
    m_rect_jump.set_pos(Vector(width - height * BUTTON_SCALE, height - height * BUTTON_SCALE));
    m_draw_jump = m_rect_jump.grown(-m_rect_jump.get_height() * 3 / 8);

    m_rect_action.set_size(height * BUTTON_SCALE, height * BUTTON_SCALE);
    m_rect_action.set_pos(Vector(width - 2 * height * BUTTON_SCALE, height - height * BUTTON_SCALE));
    m_draw_action = m_rect_action.grown(-m_rect_action.get_height() * 3 / 8);

    m_rect_escape.set_size(height * BUTTON_SCALE / 2, height * BUTTON_SCALE / 2);
    m_rect_escape.set_pos(Vector(0, 0));
    m_draw_escape = m_rect_escape.grown(-m_rect_escape.get_height() / 4);

    m_rect_cheats.set_size(height * BUTTON_SCALE / 2, height * BUTTON_SCALE / 2);
    m_rect_cheats.set_pos(Vector(width - 2 * height * BUTTON_SCALE / 2, 0));
    m_draw_cheats = m_rect_cheats.grown(-m_rect_cheats.get_height() / 4);

    m_rect_debug.set_size(height * BUTTON_SCALE / 2, height * BUTTON_SCALE / 2);
    m_rect_debug.set_pos(Vector(width - height * BUTTON_SCALE / 2, 0));
    m_draw_debug = m_rect_debug.grown(-m_rect_debug.get_height() / 4);
  }

  PaintStyle translucent;
  translucent.set_alpha(0.5f);

  context.color().draw_surface_scaled(m_tex_dirs, m_draw_directions, LAYER_GUI + 99, translucent);

  if (m_up)
    context.color().draw_surface_scaled(m_tex_up, m_draw_directions, LAYER_GUI + 99, translucent);
  if (m_down)
    context.color().draw_surface_scaled(m_tex_dwn, m_draw_directions, LAYER_GUI + 99, translucent);
  if (m_left)
    context.color().draw_surface_scaled(m_tex_lft, m_draw_directions, LAYER_GUI + 99, translucent);
  if (m_right)
    context.color().draw_surface_scaled(m_tex_rgt, m_draw_directions, LAYER_GUI + 99, translucent);

  context.color().draw_surface_scaled(m_action ? m_tex_btn_press : m_tex_btn, m_draw_action, LAYER_GUI + 99, translucent);
  context.color().draw_surface_scaled(m_tex_action, m_draw_action, LAYER_GUI + 99, translucent);

  context.color().draw_surface_scaled(m_jump ? m_tex_btn_press : m_tex_btn, m_draw_jump, LAYER_GUI + 99, translucent);
  context.color().draw_surface_scaled(m_tex_jump, m_draw_jump, LAYER_GUI + 99, translucent);

  context.color().draw_surface_scaled(m_escape ? m_tex_btn_press : m_tex_btn, m_draw_escape, LAYER_GUI + 99, translucent);
  context.color().draw_surface_scaled(m_tex_pause, m_draw_escape.grown(-m_draw_escape.get_height() / 8), LAYER_GUI + 99, translucent);

  if (g_config->developer_mode)
  {
    context.color().draw_surface_scaled(m_cheats ? m_tex_btn_press : m_tex_btn, m_draw_cheats, LAYER_GUI + 99, translucent);
    context.color().draw_surface_scaled(m_tex_cheats, m_draw_cheats, LAYER_GUI + 99, translucent);

    context.color().draw_surface_scaled(m_debug ? m_tex_btn_press : m_tex_btn, m_draw_debug, LAYER_GUI + 99, translucent);
    context.color().draw_surface_scaled(m_tex_debug, m_draw_debug, LAYER_GUI + 99, translucent);
  }
}

void
MobileController::update()
{
  if (!g_config->mobile_controls)
    return;

  m_old_up = m_up;
  m_old_down = m_down;
  m_old_left = m_left;
  m_old_right = m_right;
  m_old_jump = m_jump;
  m_old_action = m_action;
  m_old_cheats = m_cheats;
  m_old_debug = m_debug;
  m_old_escape = m_escape;

  m_up = m_down = m_left = m_right = m_jump = m_action = m_cheats = m_debug = m_escape = false;

  // Allow using on-screen controls with the mouse
  int x, y;
  auto buttons = SDL_GetMouseState(&x, &y);
  if ((buttons & SDL_BUTTON_LMASK) != 0)
  {
    activate_widget_at_pos(static_cast<float>(x), static_cast<float>(y));
  }

  for (auto& i : m_fingers)
  {
    activate_widget_at_pos(i.second.x, i.second.y);
  }
}

void
MobileController::apply(Controller& controller) const
{
  if (!g_config->mobile_controls)
    return;

  if (m_up != m_old_up)
    controller.set_control(Control::UP, m_up);
  if (m_down != m_old_down)
    controller.set_control(Control::DOWN, m_down);
  if (m_left != m_old_left)
    controller.set_control(Control::LEFT, m_left);
  if (m_right != m_old_right)
    controller.set_control(Control::RIGHT, m_right);
  if (m_jump != m_old_jump)
    controller.set_control(Control::JUMP, m_jump);
  if (m_action != m_old_action)
    controller.set_control(Control::ACTION, m_action);
  if (m_cheats != m_old_cheats)
    controller.set_control(Control::CHEAT_MENU, m_cheats);
  if (m_debug != m_old_debug)
    controller.set_control(Control::DEBUG_MENU, m_debug);
  if (m_escape != m_old_escape)
    controller.set_control(Control::ESCAPE, m_escape);

  if (m_up || m_down || m_left || m_right || m_jump || m_action || m_cheats || m_debug || m_escape)
  {
    controller.set_touchscreen(true);
  }
}

bool
MobileController::process_finger_down_event(const SDL_TouchFingerEvent& event)
{
  Vector pos(event.x * float(m_screen_width), event.y * float(m_screen_height));
  m_fingers[event.fingerId] = pos;
  return m_rect_jump.contains(pos) ||
    m_rect_action.contains(pos) ||
    m_rect_escape.contains(pos) ||
    m_rect_directions.contains(pos) ||
    (g_config->developer_mode && m_rect_cheats.contains(pos)) ||
    (g_config->developer_mode && m_rect_debug.contains(pos));
}

bool
MobileController::process_finger_up_event(const SDL_TouchFingerEvent& event)
{
  Vector pos(event.x * float(m_screen_width), event.y * float(m_screen_height));
  m_fingers.erase(event.fingerId);
  return m_rect_jump.contains(pos) ||
    m_rect_action.contains(pos) ||
    m_rect_escape.contains(pos) ||
    m_rect_directions.contains(pos) ||
    (g_config->developer_mode && m_rect_cheats.contains(pos)) ||
    (g_config->developer_mode && m_rect_debug.contains(pos));
}

bool
MobileController::process_finger_motion_event(const SDL_TouchFingerEvent& event)
{
  Vector pos(event.x * float(m_screen_width), event.y * float(m_screen_height));
  m_fingers[event.fingerId] = pos;
  return m_rect_jump.contains(pos) ||
    m_rect_action.contains(pos) ||
    m_rect_escape.contains(pos) ||
    m_rect_directions.contains(pos) ||
    (g_config->developer_mode && m_rect_cheats.contains(pos)) ||
    (g_config->developer_mode && m_rect_debug.contains(pos));
}

void
MobileController::activate_widget_at_pos(float x, float y)
{
  if (!g_config->mobile_controls)
    return;

  Vector pos(x, y);

  if (m_rect_jump.contains(pos))
    m_jump = true;

  if (m_rect_action.contains(pos))
    m_action = true;

  if (g_config->developer_mode)
  {
    if (m_rect_cheats.contains(pos))
      m_cheats = true;

    if (m_rect_debug.contains(pos))
      m_debug = true;
  }

  if (m_rect_escape.contains(pos))
    m_escape = true;

  Rectf up = m_rect_directions;
  up.set_bottom(up.get_bottom() - up.get_height() * 2.f / 3.f);
  if (up.contains(pos))
    m_up = true;

  Rectf down = m_rect_directions;
  down.set_top(down.get_top() + down.get_height() * 2.f / 3.f);
  if (down.contains(pos))
    m_down = true;

  Rectf left = m_rect_directions;
  left.set_right(left.get_right() - left.get_width() * 7.f / 12.f);
  if (left.contains(pos))
    m_left = true;

  Rectf right = m_rect_directions;
  right.set_left(right.get_left() + right.get_width() * 7.f / 12.f);
  if (right.contains(pos))
    m_right = true;
}


/* EOF */
