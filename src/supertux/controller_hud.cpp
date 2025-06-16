//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/controller_hud.hpp"

#include "control/input_manager.hpp"
#include "math/vector.hpp"
#include "video/drawing_context.hpp"

ControllerHUD::ControllerHUD() :
  m_controls()
{
  const Sizef btn_size(16, 16);
  const Vector pos(128, 64);

  const Vector dpad_pos = pos + Vector(-64.0f, 0.0f);
  m_controls[Control::LEFT] = Rectf::from_center(dpad_pos + Vector(-16.0f, 0.0f), btn_size);
  m_controls[Control::RIGHT] = Rectf::from_center(dpad_pos + Vector(16.0f, 0.0f), btn_size);
  m_controls[Control::UP] = Rectf::from_center(dpad_pos + Vector(0.0f, -16.0f), btn_size);
  m_controls[Control::DOWN] = Rectf::from_center(dpad_pos + Vector(0.0f, 16.0f), btn_size);

  const Vector peek_pos = pos + Vector(0.0f, -24.0f);
  m_controls[Control::PEEK_LEFT] = Rectf::from_center(peek_pos + Vector(-16.0f, 0.0f), btn_size);
  m_controls[Control::PEEK_RIGHT] = Rectf::from_center(peek_pos + Vector(16.0f, 0.0f), btn_size);
  m_controls[Control::PEEK_UP] = Rectf::from_center(peek_pos + Vector(0.0f, -16.0f), btn_size);
  m_controls[Control::PEEK_DOWN] = Rectf::from_center(peek_pos + Vector(0.0f, 16.0f), btn_size);

  const Vector btn_pos = pos + Vector(64.0f, -8.0f);
  m_controls[Control::ACTION] = Rectf::from_center(btn_pos + Vector(-20.0f, 0.0f), btn_size);
  m_controls[Control::JUMP] = Rectf::from_center(btn_pos + Vector(0.0f, 20.0f), btn_size);

  m_controls[Control::ITEM] = Rectf::from_center(pos + Vector(-24.f, 24.0f), Sizef(16, 8));
  m_controls[Control::ESCAPE] = Rectf::from_center(pos + Vector(0.0f, 24.0f), Sizef(16, 8));
  m_controls[Control::START] = Rectf::from_center(pos + Vector(24.0f, 24.0f), Sizef(16, 8));

}

void
ControllerHUD::draw(DrawingContext& context)
{
  Canvas& canvas = context.color();
  Controller& controller = InputManager::current()->get_controller();

  for(const auto& control: m_controls)
  {
    if (controller.pressed(control.first)) {
      canvas.draw_filled_rect(control.second, Color::WHITE, LAYER_HUD);
    } else if (controller.released(control.first)) {
      canvas.draw_filled_rect(control.second, Color::BLACK, LAYER_HUD);
    } else if (controller.hold(control.first)) {
      canvas.draw_filled_rect(control.second, Color::RED, LAYER_HUD);
    } else {
      canvas.draw_filled_rect(control.second, Color(0.5f, 0.5f, 0.5f), LAYER_HUD);
    }
  }
}

/* EOF */
