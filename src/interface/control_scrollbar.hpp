//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_SCROLLBAR_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_SCROLLBAR_HPP

#include "editor/widget.hpp"

#include "math/rectf.hpp"
#include "math/vector.hpp"

/** A scrollbar, which manages a scroll progress variable. */
class ControlScrollbar final : public Widget
{
public:
  ControlScrollbar(float total_region, float covered_region,
                   float& progress, float mouse_wheel_speed = 10.f);

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;

  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;
  virtual bool on_mouse_wheel(const SDL_MouseWheelEvent& wheel) override;

  bool is_active() const;

  void scroll(float amount);

  inline void set_mouse_wheel_speed(float speed) { m_mouse_wheel_speed = speed; }

  inline void set_rect(const Rectf& rect) { m_rect = rect; }
  inline const Rectf& get_rect() const { return m_rect; }

  inline float get_covered_region() const { return m_covered_region; }
  inline float get_total_region() const { return m_total_region; }

  inline void set_covered_region(float region) { m_covered_region = region; check_regions(); }
  inline void set_total_region(float region) { m_total_region = region; check_regions(); }

private:
  void check_regions();

  Rectf get_bar_rect() const;

private:
  /** Determines the scroll speed, when using the mouse wheel. */
  float m_mouse_wheel_speed;

  /** Whether or not the mouse is clicking on the bar */
  bool m_scrolling;

  /** Whether or not the mouse hovers above the bar */
  bool m_hovering;

  /** The length (height) of the region to scroll */
  float m_total_region;

  /** The length (height) of the viewport for the region */
  float m_covered_region;

  /** The length (height) between the beginning of the viewport and the beginning of the region */
  float& m_progress;

  /** The logical position and size of the widget */
  Rectf m_rect;
  
  /** `true` if the scroller is horizontal; `false` if it is vertical */
  //bool is_horizontal;

  float m_last_mouse_pos;
  //float zoom_factor;

private:
  ControlScrollbar(const ControlScrollbar&) = delete;
  ControlScrollbar& operator=(const ControlScrollbar&) = delete;
};

#endif

/* EOF */
