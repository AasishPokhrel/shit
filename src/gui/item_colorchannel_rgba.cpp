//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "gui/item_colorchannel_rgba.hpp"

#include <sstream>

#include "math/util.hpp"
#include "supertux/resources.hpp"
#include "video/drawing_context.hpp"

namespace {

std::string colour_value_to_string(float v_raw, bool is_linear)
{
  float v = v_raw;
  if (!is_linear)
    v = Color::remove_gamma(v);
  v *= 100.0f;
  // not using std::to_string() as it padds the end with '0's
  v = 0.01f * floorf(v * 100.0f + 0.5f);
  std::ostringstream os;
  os << v_raw << " (" << v << " %" << ")";
  return os.str();
}

std::string float_to_string(float v)
{
  std::ostringstream os;
  os << v;
  return os.str();
}

} // namespace

ItemColorChannelRGBA::ItemColorChannelRGBA(float* input, Color channel, int id,
    bool is_linear) :
  MenuItem(colour_value_to_string(*input, is_linear), id),
  m_number(input),
  m_number_prev(*input),
  m_is_linear(is_linear),
  m_edit_mode(false),
  m_flickw(static_cast<int>(Resources::normal_font->get_text_width("_"))),
  m_channel(channel)
{
}

void
ItemColorChannelRGBA::draw(DrawingContext& context, const Vector& pos,
  int menu_width, bool active)
{
  if (!m_edit_mode && *m_number != m_number_prev) {
    set_text(colour_value_to_string(*m_number, m_is_linear));
    m_number_prev = *m_number;
  }

  MenuItem::draw(context, pos, menu_width, active);
  const float lw = float(menu_width - 32) * (*m_number);
  context.color().draw_filled_rect(Rectf(pos + Vector(16, -4),
                                         pos + Vector(16 + lw, 4)),
                                   m_channel, 0.0f, LAYER_GUI-1);
}

int
ItemColorChannelRGBA::get_width() const
{
  return static_cast<int>(Resources::normal_font->get_text_width(get_text()) + 16 + static_cast<float>(m_flickw));
}

void
ItemColorChannelRGBA::enable_edit_mode()
{
  if (m_edit_mode)
    // Do nothing if it is already enabled
    return;
  m_edit_mode = true;
  set_text(float_to_string(*m_number));
}


void
ItemColorChannelRGBA::event(const SDL_Event& ev)
{
  if (ev.type == SDL_TEXTINPUT) {
    std::string txt = ev.text.text;
    for (auto& c : txt) {
      add_char(c);
    }
  }
}

void
ItemColorChannelRGBA::add_char(char c)
{
  enable_edit_mode();
  std::string text = get_text();

  if (c == '.' || c == ',')
  {
    const bool has_comma = (text.find('.') != std::string::npos);
    if (!has_comma)
    {
      if (text.empty()) {
        text = "0.";
      } else {
        text.push_back('.');
      }
    }
  }
  else if (isdigit(c))
  {
    text.push_back(c);
  }
  else
  {
    return;
  }

  float number = std::stof(text);
  if (0.0f <= number && number <= 1.0f) {
    *m_number = number;
    set_text(text);
  }
}

void
ItemColorChannelRGBA::remove_char()
{
  enable_edit_mode();
  std::string text = get_text();

  if (text.empty())
  {
    *m_number = 0.0f;
  }
  else
  {
    text.pop_back();

    if (!text.empty()) {
      *m_number = std::stof(text);
    } else {
      *m_number = 0.0f;
    }
  }

  set_text(text);
}

void
ItemColorChannelRGBA::process_action(const MenuAction& action)
{
  switch (action)
  {
    case MenuAction::REMOVE:
      remove_char();
      break;

    case MenuAction::LEFT:
      *m_number = roundf(*m_number * 10.0f) / 10.0f;
      *m_number -= 0.1f;
      *m_number = math::clamp(*m_number, 0.0f, 1.0f);
      m_edit_mode = false;
      break;

    case MenuAction::RIGHT:
      *m_number = roundf(*m_number * 10.0f) / 10.0f;
      *m_number += 0.1f;
      *m_number = math::clamp(*m_number, 0.0f, 1.0f);
      m_edit_mode = false;
      break;

    case MenuAction::UNSELECT:
      m_edit_mode = false;
      break;

    default:
      break;
  }
}

Color
ItemColorChannelRGBA::get_color() const
{
  return m_channel;
}

/* EOF */
