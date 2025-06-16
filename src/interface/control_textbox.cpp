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

#include "interface/control_textbox.hpp"

#include <math.h>

#include <SDL.h>

#include "math/vector.hpp"
#include "math/rectf.hpp"
#include "supertux/resources.hpp"
#include "util/log.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

// The time for the caret to change visibility (when it flashes).
static const float CONTROL_CURSOR_TIMER = 0.5f;

ControlTextbox::ControlTextbox() :
  m_validate_string(),
  m_charlist(),
  m_string(nullptr),
  m_internal_string_backup(""),
  m_cursor_timer(CONTROL_CURSOR_TIMER),
  m_caret_pos(),
  m_secondary_caret_pos(),
  m_shift_pressed(false),
  m_ctrl_pressed(false),
  m_mouse_pressed(),
  m_current_offset(0)
{
}

void
ControlTextbox::update(float dt_sec)
{
  m_cursor_timer -= dt_sec;
  if (m_cursor_timer < -CONTROL_CURSOR_TIMER) {
    m_cursor_timer = CONTROL_CURSOR_TIMER;
  }

  // Apparently the stuff bugs from time to time.
  recenter_offset();
}

void
ControlTextbox::delete_char_before_caret()
{
  if (m_charlist.size()) {
    m_caret_pos--;
    m_secondary_caret_pos = m_caret_pos;
    m_cursor_timer = CONTROL_CURSOR_TIMER;

    auto it = m_charlist.begin();
    advance(it, m_caret_pos);
    m_charlist.erase(it);

    recenter_offset();
  }
}

void
ControlTextbox::draw(DrawingContext& context)
{
  InterfaceControl::draw(context);

  context.color().draw_filled_rect(m_rect,
                                   m_has_focus ? Color(0.75f, 0.75f, 0.7f, 1.f)
                                               : Color(0.5f, 0.5f, 0.5f, 1.f),
                                   LAYER_GUI);

  if (m_caret_pos != m_secondary_caret_pos) {
    float lgt1 = Resources::control_font
                 ->get_text_width(get_first_chars_visible(std::max(
                                std::min(m_caret_pos, m_secondary_caret_pos) - m_current_offset,
                                0
                                )));

    float lgt2 = Resources::control_font
                 ->get_text_width(get_first_chars_visible(std::min(
                                 std::max(m_caret_pos, m_secondary_caret_pos) - m_current_offset,
                                 int(get_contents_visible().size())
                                 )));

    context.color().draw_filled_rect(Rectf(m_rect.p1() + Vector(lgt1 + 5.f, 0.f),
                                           m_rect.p1() + Vector(lgt2 + 5.f, m_rect.get_height())
                                           ),
                                     m_has_focus ? Color(1.f, 1.f, .9f, 0.75f)
                                                 : Color(1.f, 1.f, .9f, 0.5f),
                                     LAYER_GUI);
  }

  context.color().draw_text(Resources::control_font,
                            get_contents_visible(),
                            Vector(m_rect.get_left() + 5.f,
                                   (m_rect.get_top() + m_rect.get_bottom()) / 2 -
                                    Resources::control_font->get_height() / 2),
                            FontAlignment::ALIGN_LEFT,
                            LAYER_GUI + 1,
                            Color::BLACK);
  if (m_cursor_timer > 0 && m_has_focus) {
    float lgt = Resources::control_font
                                ->get_text_width(get_first_chars_visible(m_caret_pos - m_current_offset));

    context.color().draw_line(m_rect.p1() + Vector(lgt + 5.f, 2.f),
                              m_rect.p1() + Vector(lgt + 5.f,
                                  Resources::control_font->get_height() + 4.f),
                              Color::BLACK,
                              LAYER_GUI + 1);
  }
}

bool
ControlTextbox::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(button.x, button.y);
  if (m_rect.contains(mouse_pos)) {
    m_has_focus = true;
    m_cursor_timer = CONTROL_CURSOR_TIMER;
    m_caret_pos = get_text_position(mouse_pos);
    m_secondary_caret_pos = m_caret_pos;
    m_mouse_pressed = true;
    return true;
  } else {
    if (m_has_focus) {
      parse_value();
    }
    m_has_focus = false;
  }
  return false;
}

bool
ControlTextbox::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  if (m_mouse_pressed) {
    m_mouse_pressed = false;
    return true;
  }
  return false;
}

bool
ControlTextbox::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  InterfaceControl::on_mouse_motion(motion);

  Vector mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  if (m_mouse_pressed) {
    m_cursor_timer = CONTROL_CURSOR_TIMER;
    m_caret_pos = get_text_position(mouse_pos);
    return true;
  }
  return false;
}

bool
ControlTextbox::on_key_up(const SDL_KeyboardEvent& key)
{

  if (m_has_focus)
  {
    if (key.keysym.sym == SDLK_LSHIFT || key.keysym.sym == SDLK_RSHIFT)
    {
      m_shift_pressed = false;
      return true;
    }
    else if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
    {
      m_ctrl_pressed = false;
      return true;
    }
  }

  return false;
}

bool
ControlTextbox::on_key_down(const SDL_KeyboardEvent& key)
{
  if (!m_has_focus)
    return false;

  if (key.keysym.sym == SDLK_LEFT && m_caret_pos > 0)
  {
    if (!m_shift_pressed && m_secondary_caret_pos != m_caret_pos)
    {
      m_secondary_caret_pos = m_caret_pos = std::min(m_secondary_caret_pos, m_caret_pos);
    }
    else
    {
      m_caret_pos--;
      m_cursor_timer = CONTROL_CURSOR_TIMER;
      if (!m_shift_pressed)
        m_secondary_caret_pos = m_caret_pos;
    }

    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_RIGHT && m_caret_pos < int(m_charlist.size()))
  {
    if (!m_shift_pressed && m_secondary_caret_pos != m_caret_pos)
    {
      m_secondary_caret_pos = m_caret_pos = std::max(m_secondary_caret_pos, m_caret_pos);
    }
    else
    {
      m_caret_pos++;
      m_cursor_timer = CONTROL_CURSOR_TIMER;
      if (!m_shift_pressed)
        m_secondary_caret_pos = m_caret_pos;
    }

    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_BACKSPACE)
  {
    if (!erase_selected_text() && m_caret_pos > 0)
    {
      delete_char_before_caret();
    }
    return true;
  }
  else if (key.keysym.sym == SDLK_DELETE)
  {
    if (!erase_selected_text() && static_cast<int>(m_charlist.size()) > m_caret_pos)
    {
      m_caret_pos++;
      delete_char_before_caret();
    }
    return true;
  }
  else if (key.keysym.sym == SDLK_HOME)
  {
    m_caret_pos = 0;
    m_cursor_timer = CONTROL_CURSOR_TIMER;
    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_END)
  {
    m_caret_pos = int(m_charlist.size());
    m_cursor_timer = CONTROL_CURSOR_TIMER;
    if (!m_shift_pressed)
      m_secondary_caret_pos = m_caret_pos;

    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_LSHIFT || key.keysym.sym == SDLK_RSHIFT)
  {
    m_shift_pressed = true;
    return true;
  }
  else if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
  {
    m_ctrl_pressed = true;
    return true;
  }
  else if (key.keysym.sym == SDLK_c && m_ctrl_pressed)
  {
    copy();
    return true;
  }
  else if (key.keysym.sym == SDLK_v && m_ctrl_pressed)
  {
    paste();
    return true;
  }
  else if (key.keysym.sym == SDLK_a && m_ctrl_pressed)
  {
    m_caret_pos = 0;
    m_secondary_caret_pos = static_cast<int>(m_charlist.size());
    recenter_offset();
    return true;
  }
  else if (key.keysym.sym == SDLK_RETURN)
  {
    parse_value();
    return true;
  }

  return false;
}

bool
ControlTextbox::event(const SDL_Event& ev) {
  Widget::event(ev);

  if (ev.type == SDL_TEXTINPUT && m_has_focus)
    put_text(std::string(ev.text.text));

  return false;
}

bool
ControlTextbox::parse_value(bool call_on_change /* = true  (see header)*/)
{
  // Abort if we have a validation function for the string, and the function
  // says the string is invalid.
  if (m_validate_string) {
    if (!m_validate_string(this, get_contents())) {
      revert_value();
      return false;
    }
  }

  std::string new_str = get_string();
  if (m_internal_string_backup != new_str) {
    m_internal_string_backup = new_str;

    if (m_string)
      *m_string = new_str;

    if (call_on_change && m_on_change)
      m_on_change();
  }

  return true;
}

void
ControlTextbox::revert_value()
{
  std::string str = m_internal_string_backup;

  m_charlist.clear();
  for (char c : str) {
    m_charlist.push_back(c);
  }

  m_caret_pos = 0;
  m_secondary_caret_pos = 0;

  recenter_offset();
}

const std::string&
ControlTextbox::get_string() const
{
  return m_internal_string_backup;
}

std::string
ControlTextbox::get_contents() const
{
  std::string temp;

  for (char c : m_charlist) {
    temp += c;
  }

  return temp;
}

std::string
ControlTextbox::get_first_chars(int amount) const
{
  std::string temp;

  for (char c : m_charlist) {
    if (!(amount--)) break;
    temp += c;
  }

  return temp;
}

std::string
ControlTextbox::get_contents_visible() const
{
  std::string temp;
  int remaining = m_current_offset;

  for (char c : m_charlist) {
    if (--remaining < 0) {
      temp += c;
    }
  }

  return get_truncated_text(temp);
}

std::string
ControlTextbox::get_first_chars_visible(int amount) const
{
  return get_contents_visible().substr(0, amount);
}

int
ControlTextbox::get_text_position(const Vector& pos) const
{
  float dist = pos.x - m_rect.get_left();
  int i = 0;

  while (Resources::control_font->get_text_width(get_first_chars_visible(i)) < dist
         && i <= int(m_charlist.size()))
    i++;

  return std::max(i - 1 + m_current_offset, 0);
}

std::string
ControlTextbox::get_truncated_text(const std::string& text) const
{
  if (fits(text)) return text;

  std::string temp = text;
  while (!temp.empty() && !fits(temp))
    temp.pop_back();

  return temp;
}

bool
ControlTextbox::fits(const std::string& text) const
{
  return Resources::control_font->get_text_width(text) <= m_rect.get_width() - 10.f;
}

void
ControlTextbox::recenter_offset()
{
  while (m_caret_pos < m_current_offset && m_current_offset > 0) {
    m_current_offset--;
  }

  while (m_caret_pos > m_current_offset + int(get_contents_visible().size()) && m_current_offset < int(get_contents().size())) {
    m_current_offset++;
  }

  while (m_current_offset > 0 && fits(get_contents().substr(m_current_offset - 1))) {
    m_current_offset--;
  }
}

bool
ControlTextbox::copy() const
{
  if (m_caret_pos == m_secondary_caret_pos)
    return false;

  SDL_ClearError();
  int ret = SDL_SetClipboardText(get_selected_text().c_str());

  if (ret)
    log_warning << "Couldn't copy text to clipboard: " << SDL_GetError() << std::endl;

  return ret == 0;
}

bool
ControlTextbox::paste()
{
  if (!SDL_HasClipboardText())
    return false;

  char* txt = SDL_GetClipboardText();

  if (txt)
    put_text(std::string(txt));
  else
    log_warning << "Couldn't paste text from clipboard: " << SDL_GetError() << std::endl;

  return txt != nullptr;
}

std::string
ControlTextbox::get_selected_text() const
{
  // There's probably a cleaner way to do this...
  std::string temp;
  int start = std::min(m_caret_pos, m_secondary_caret_pos),
      end = std::max(m_caret_pos, m_secondary_caret_pos),
      pos = 0;

  for (char c : m_charlist)
  {
    if (pos++ < start)
      continue;

    if (pos > end)
      break;

    temp += c;
  }

  return temp;
}

bool
ControlTextbox::put_text(const std::string& text)
{
  bool has_erased_text = erase_selected_text();

  for (char c : text)
  {
    auto it = m_charlist.begin();
    advance(it, m_caret_pos);
    m_charlist.insert(it, c);

    m_caret_pos++;
    m_secondary_caret_pos = m_caret_pos;
    m_cursor_timer = CONTROL_CURSOR_TIMER;
  }

  recenter_offset();

  return has_erased_text;
}

bool
ControlTextbox::erase_selected_text()
{
  if (m_secondary_caret_pos == m_caret_pos)
    return false;

  m_cursor_timer = CONTROL_CURSOR_TIMER;

  auto it = m_charlist.begin();
  advance(it, std::min(m_caret_pos, m_secondary_caret_pos));
  auto it2 = m_charlist.begin();
  advance(it2, std::max(m_caret_pos, m_secondary_caret_pos));
  m_charlist.erase(it, it2);

  m_caret_pos = std::min(m_caret_pos, m_secondary_caret_pos);
  m_secondary_caret_pos = m_caret_pos;
  return true;
}

/* EOF */
