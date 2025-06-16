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

#ifndef HEADER_SUPERTUX_EDITOR_BUTTON_WIDGET_HPP
#define HEADER_SUPERTUX_EDITOR_BUTTON_WIDGET_HPP

#include "editor/widget.hpp"

#include <functional>

#include "math/rectf.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"

class ButtonWidget : public Widget
{
private:
public:
  ButtonWidget(SpritePtr sprite, const Vector& pos, std::function<void()> m_sig_click = {});
  ButtonWidget(const std::string& path, const Vector& pos, std::function<void()> callback = {}) :
    ButtonWidget(SpriteManager::current()->create(path), pos, std::move(callback))
  {
  }

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;

  virtual void setup() override;
  virtual void on_window_resize() override;

  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;

private:
  SpritePtr m_sprite;
  Rectf m_rect;
  bool m_grab;
  bool m_hover;
  std::function<void()> m_sig_click;

private:
  ButtonWidget(const ButtonWidget&) = delete;
  ButtonWidget& operator=(const ButtonWidget&) = delete;
};

#endif

/* EOF */
