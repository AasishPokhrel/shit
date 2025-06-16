//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//                2022 Vankata453
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

#include "gui/item_textfield.hpp"

#include "supertux/colorscheme.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "util/string_util.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

ItemTextField::ItemTextField(const std::string& text_, std::string* input_, int id_) :
  MenuItem(text_, id_),
  input(input_),
  m_input_undo(),
  m_input_redo(),
  m_cursor("|"),
  m_cursor_width(Resources::normal_font->get_text_width(m_cursor)),
  m_cursor_left_offset(0)
{
}

void
ItemTextField::draw(DrawingContext& context, const Vector& pos, int menu_width, bool active)
{
  const int index = active ? static_cast<int>(input->size()) - m_cursor_left_offset : -1;
  const std::string input_part_1 = active ? input->substr(0, index) : *input;
  const std::string input_part_2 = active ? input->substr(index) : "";
  const float input_part_2_width = Resources::normal_font->get_text_width(input_part_2);
  context.color().draw_text(Resources::normal_font, input_part_1,
                            Vector(pos.x + static_cast<float>(menu_width) - 9.0f - input_part_2_width - m_cursor_width,
                                   pos.y - Resources::normal_font->get_height() / 2.0f),
                            ALIGN_RIGHT, LAYER_GUI, ColorScheme::Menu::field_color);
  if (active && ((int(g_real_time * 2) % 2) || (m_cursor_left_offset != 0 && m_cursor_left_offset != static_cast<int>(input->size()))))
  {
    // Draw text cursor.
    context.color().draw_text(Resources::normal_font, m_cursor,
                              Vector(pos.x + static_cast<float>(menu_width) - 12.0f - input_part_2_width,
                                     pos.y - Resources::normal_font->get_height() / 2.0f),
                              ALIGN_RIGHT, LAYER_GUI, Color::CYAN);
  }
  context.color().draw_text(Resources::normal_font, input_part_2,
                            Vector(pos.x + static_cast<float>(menu_width) - 16.0f,
                                   pos.y - Resources::normal_font->get_height() / 2.0f),
                            ALIGN_RIGHT, LAYER_GUI, ColorScheme::Menu::field_color);
  context.color().draw_text(Resources::normal_font, get_text(),
                            Vector(pos.x + 16.0f,
                                   pos.y - static_cast<float>(Resources::normal_font->get_height()) / 2.0f),
                            ALIGN_LEFT, LAYER_GUI, active ? g_config->activetextcolor : get_color());
}

int
ItemTextField::get_width() const
{
  return static_cast<int>(Resources::normal_font->get_text_width(get_text()) + Resources::normal_font->get_text_width(*input) + 16.0f + m_cursor_width);
}

void
ItemTextField::event(const SDL_Event& ev)
{
  if (ev.type == SDL_TEXTINPUT) // Text input
  {
    insert_text(ev.text.text, m_cursor_left_offset);
  }
  else if (ev.type == SDL_KEYDOWN)
  {
    if (ev.key.keysym.sym == SDLK_DELETE) // Delete back
    {
      delete_back();
    }
    else if (ev.key.keysym.sym == SDLK_HOME) // Home: go to beginning of text
    {
      go_to_beginning();
    }
    else if (ev.key.keysym.sym == SDLK_END) // End: go to end of text
    {
      go_to_end();
    }
    else if (SDL_GetModState() & KMOD_CTRL) //Commands which require CTRL
    {
      if (ev.key.keysym.sym == SDLK_x) // Cut (whole line)
      {
        cut();
      }
      else if (ev.key.keysym.sym == SDLK_c) // Copy (whole line)
      {
        copy();
      }
      else if (ev.key.keysym.sym == SDLK_v) // Paste
      {
        paste();
      }
      else if (ev.key.keysym.sym == SDLK_z) // Undo
      {
        undo();
      }
      else if (ev.key.keysym.sym == SDLK_y) // Redo
      {
        redo();
      }
    }
  }
}

void
ItemTextField::process_action(const MenuAction& action)
{
  if (action == MenuAction::REMOVE) // Delete front (backspace)
  {
    delete_front();
  }
  else if (action == MenuAction::LEFT) // Left
  {
    go_left();
  }
  else if (action == MenuAction::RIGHT) // Right
  {
    go_right();
  }
}

void
ItemTextField::update_undo()
{
  m_input_undo = *input;
  m_input_redo.clear();
}

// Text manipulation and navigation functions

void
ItemTextField::insert_text(const std::string& text, const int left_offset_pos)
{
  update_undo();
  *input = input->substr(0, input->size() - left_offset_pos) + text +
    input->substr(input->size() - left_offset_pos);
  on_input_update();
}

void
ItemTextField::clear()
{
  m_cursor_left_offset = 0;
  input->clear();
  on_input_update();
}

void
ItemTextField::go_left()
{
  if (m_cursor_left_offset >= static_cast<int>(input->size()))
    return;

  unsigned char last_char;
  do
  {
    last_char = input->at(input->size() - m_cursor_left_offset - 1);
    m_cursor_left_offset++;
    if (m_cursor_left_offset >= static_cast<int>(input->size())) break;
  } while ((last_char & 128) && !(last_char & 64));
}

void
ItemTextField::go_right()
{
  if (m_cursor_left_offset <= 0)
    return;

  unsigned char next_char = '\0';
  do
  {
    next_char = m_cursor_left_offset == 1 ? next_char : input->at(input->size() - m_cursor_left_offset + 1);
    m_cursor_left_offset--;
    if (m_cursor_left_offset <= 0) break;
  } while ((next_char & 128) && !(next_char & 64));
}

void
ItemTextField::go_to_beginning()
{
  m_cursor_left_offset = static_cast<int>(input->size());
}

void
ItemTextField::go_to_end()
{
  m_cursor_left_offset = 0;
}

void
ItemTextField::delete_front()
{
  if (!input->empty() && m_cursor_left_offset < static_cast<int>(input->size()))
  {
    update_undo();
    unsigned char last_char;
    do
    {
      const int index = static_cast<int>(input->size()) - m_cursor_left_offset - 1;
      last_char = input->at(index);
      *input = input->substr(0, index) +
        input->substr(input->size() - m_cursor_left_offset);
      if (input->empty() || m_cursor_left_offset >= static_cast<int>(input->size())) break;
    } while ((last_char & 128) && !(last_char & 64));
    on_input_update();
  }
  else
  {
    invalid_remove();
  }
}

void
ItemTextField::delete_back()
{
  if (!input->empty() && m_cursor_left_offset > 0)
  {
    update_undo();
    unsigned char next_char = '\0';
    do
    {
      next_char = m_cursor_left_offset == 1 ? next_char : input->at(input->size() - m_cursor_left_offset + 1);
      *input = input->substr(0, input->size() - m_cursor_left_offset) +
        input->substr(input->size() - m_cursor_left_offset + 1);
      m_cursor_left_offset--;
      if (input->empty() || m_cursor_left_offset <= 0) break;
    } while ((next_char & 128) && !(next_char & 64));
    on_input_update();
  }
  else
  {
    invalid_remove();
  }
}

void
ItemTextField::cut()
{
  update_undo();
  SDL_SetClipboardText(input->c_str());
  clear();
}

void
ItemTextField::copy()
{
  SDL_SetClipboardText(input->c_str());
}

void
ItemTextField::paste()
{
  update_undo();

  char* clipboard_content = SDL_GetClipboardText();
  std::string clipboard_text = std::string(clipboard_content);
  SDL_free(clipboard_content);
  StringUtil::replace_all(clipboard_text, "\n", " "); // Replace any newlines with spaces.

  if (clipboard_text.empty()) return;
  insert_text(clipboard_text, m_cursor_left_offset);

  on_input_update();
}

void
ItemTextField::undo()
{
  if (m_input_undo.empty()) return;
  m_input_redo = *input;
  *input = m_input_undo;
  m_input_undo.clear();

  on_input_update();
}

void
ItemTextField::redo()
{
  if (m_input_redo.empty()) return;
  m_input_undo = *input;
  *input = m_input_redo;
  m_input_redo.clear();

  on_input_update();
}

/* EOF */
