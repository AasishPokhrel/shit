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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_ENUM_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_ENUM_HPP

#include <vector>

#include "interface/control.hpp"

#include "util/log.hpp"

template<class T>
class ControlEnum : public InterfaceControl
{
public:
  ControlEnum();

  virtual void draw(DrawingContext& context) override;
  virtual bool on_mouse_button_up(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_button_down(const SDL_MouseButtonEvent& button) override;
  virtual bool on_mouse_motion(const SDL_MouseMotionEvent& motion) override;
  virtual bool on_key_up(const SDL_KeyboardEvent& key) override;
  virtual bool on_key_down(const SDL_KeyboardEvent& key) override;

  inline T get_value() const { return *m_value; }
  inline void set_value(T value) { *m_value = value; }
  inline void bind_value(T* value) { m_value = value; }

  void add_option(T key, const std::string& label) { m_options.push_back(std::make_pair(key, label)); }

  Rectf get_list_rect() const {
    return Rectf (
      m_rect.get_left(), m_rect.get_top(),
      m_rect.get_right(), m_rect.get_bottom() + m_rect.get_height() * float(m_options.size())
    );
  }

private:
  T* m_value;
  bool m_open_list;

  std::vector<std::pair<T, std::string>> m_options;
  Vector m_mouse_pos;

private:
  ControlEnum(const ControlEnum&) = delete;
  ControlEnum& operator=(const ControlEnum&) = delete;
};





// ============================================================================
// ============================================================================
// ==============================   SOURCE   ==================================
// ============================================================================
// ============================================================================

#include <math.h>

#include "math/easing.hpp"
#include "math/vector.hpp"
#include "object/custom_particle_system.hpp"
#include "supertux/resources.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

template<class T>
ControlEnum<T>::ControlEnum() :
  m_value(),
  m_open_list(false),
  m_options(),
  m_mouse_pos(0.0f, 0.0f)
{
}

template<class T>
void
ControlEnum<T>::draw(DrawingContext& context)
{
  InterfaceControl::draw(context);

  context.color().draw_filled_rect(m_rect,
                                   m_has_focus ? Color(0.75f, 0.75f, 0.7f, 1.f)
                                               : Color(0.5f, 0.5f, 0.5f, 1.f),
                                   LAYER_GUI);

  std::string label;
  auto it = std::find_if(m_options.begin(), m_options.end(), [this](const auto &a) { return a.first == *m_value; });
  if (it != m_options.end()) {
    label = it->second;
  } else {
    label = "<invalid>";
  }

  context.color().draw_text(Resources::control_font,
                            label,
                            Vector(m_rect.get_left() + 5.f,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 -
                                    Resources::control_font->get_height() / 2),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI + 1,
                            Color::BLACK);
  int i = 0;
  if (m_open_list) {
    for (const auto& option : m_options) {
      i++;
      Rectf box = m_rect.moved(Vector(0.f, m_rect.get_height() * float(i)));
      context.color().draw_filled_rect(box.grown(2.f).moved(Vector(0,4.f)), Color(0.f, 0.f, 0.f, 0.1f), 2.f, LAYER_GUI + 4);
      context.color().draw_filled_rect(box.grown(4.f).moved(Vector(0,4.f)), Color(0.f, 0.f, 0.f, 0.1f), 2.f, LAYER_GUI + 4);
      context.color().draw_filled_rect(box.grown(6.f).moved(Vector(0,4.f)), Color(0.f, 0.f, 0.f, 0.1f), 2.f, LAYER_GUI + 4);
      context.color().draw_filled_rect(box,
                                       (box.contains(m_mouse_pos)
                                         || option.first == *m_value)
                                           ? Color(0.75f, 0.75f, 0.7f, 1.f)
                                           : Color(0.5f, 0.5f, 0.5f, 1.f),
                                       LAYER_GUI + 5);

      std::string label2 = option.second;

      context.color().draw_text(Resources::control_font,
                                label2,
                                Vector(m_rect.get_left() + 5.f,
                                       (m_rect.get_top() + m_rect.get_bottom()) / 2 -
                                        Resources::control_font->get_height() / 2 +
                                        m_rect.get_height() * float(i)),
                                FontAlignment::ALIGN_LEFT,
                                LAYER_GUI + 6,
                                Color::BLACK);
    }
  }
}

template<class T>
bool
ControlEnum<T>::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  if (button.button != SDL_BUTTON_LEFT)
    return false;

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  if (m_rect.contains(mouse_pos)) {
    m_open_list = !m_open_list;
    m_has_focus = true;
    return true;
  } else if (get_list_rect().contains(mouse_pos) && m_open_list) {
    return true;
  } else {
    return false;
  }
}

template<class T>
bool
ControlEnum<T>::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  if (m_open_list) {
    if (!get_list_rect().contains(mouse_pos)) {
      m_has_focus = false;
      m_open_list = false;
    } else {
      int pos = int(floor((mouse_pos.y - m_rect.get_bottom()) / m_rect.get_height()));
      if (pos != -1) {
        // This verification shouldn't be needed but I don't trust myself
        if (pos >= 0 && pos < int(m_options.size())) {
          // Yes. We need this. Because I can't acced by numerical index.
          // There's probably a way, but I'm too bored to investigate.
          for (const auto& option : m_options) {
            if (--pos != -1) continue;
            *m_value = option.first;

            if (m_on_change)
              m_on_change();

            break;
          }
        } else {
          log_warning << "Clicked on control enum inside dropdown but at invalid position ("
                      << pos << " for a size of " << m_options.size() << ")" << std::endl;
        }
      }
      return true;
    }
  } else {
    if (!m_rect.contains(mouse_pos)) {
      m_has_focus = false;
      m_open_list = false;
    }
  }
  return false;
}

template<class T>
bool
ControlEnum<T>::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  InterfaceControl::on_mouse_motion(motion);

  m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  return false;
}

template<class T>
bool
ControlEnum<T>::on_key_up(const SDL_KeyboardEvent& key)
{
  if ((key.keysym.sym == SDLK_SPACE
    || key.keysym.sym == SDLK_RETURN
    || key.keysym.sym == SDLK_RETURN2) && m_has_focus) {
    m_open_list = !m_open_list;
    return true;
  } else {
    return false;
  }
}

template<class T>
bool
ControlEnum<T>::on_key_down(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  if (key.keysym.sym == SDLK_DOWN) {
    bool is_next = false;
    // Hacky way to get the next one in the list
    for (const auto& option : m_options) {
      if (is_next) {
        *m_value = option.first;
        is_next = false;
        break;
      } else if (option.first == *m_value) {
        is_next = true;
      }
    }

    // if we're at the last index, loop back to the beginning
    if (is_next && !m_options.empty())
      *m_value = m_options.begin()->first;

    if (m_on_change)
      m_on_change();

    return true;
  } else if (key.keysym.sym == SDLK_UP) {

    bool is_last = false;
    bool currently_on_first = true;
    T last_value = *m_value; // must assign a value else clang will complain

    // Hacky way to get the preceding one in the list
    for (const auto& option : m_options) {
      if (option.first == *m_value) {
        if (currently_on_first) {
          is_last = true;
        } else {
          *m_value = last_value;
        }
      }
      last_value = option.first;
      currently_on_first = false;
    }

    if (is_last)
      *m_value = last_value;

    if (m_on_change)
      m_on_change();

    return true;
  }

  return false;
}

#endif

/* EOF */
