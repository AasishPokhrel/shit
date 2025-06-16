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

#include "gui/item_intfield.hpp"

ItemIntField::ItemIntField(const std::string& text_, int* input_, int id_, bool positive) :
  ItemTextField(text_, new std::string, id_),
  number(input_),
  m_input(std::to_string(*input_)),
  m_positive(positive)
{
  change_input(m_input);

  // Remove minus, if the number has one, but it's only allowed to be positive.
  if (*input->begin() == '-' && m_positive)
  {
    if (m_cursor_left_offset == static_cast<int>(input->size())) m_cursor_left_offset--;
    input->erase(input->begin());
    *number *= -1;
  }
}

ItemIntField::~ItemIntField()
{
  delete input;
}

void
ItemIntField::add_char(char c, const int left_offset_pos)
{
  if (c == '-' && !m_positive)
  {
    if (!input->empty() && *input != "0")
    {
      if (*input->begin() == '-')
      {
        if (m_cursor_left_offset == static_cast<int>(input->size())) m_cursor_left_offset--;
        input->erase(input->begin());
      }
      else
      {
        input->insert(input->begin(), '-');
      }
    }
    else
    {
      *input = "-";
    }
  }

  if (c < '0' || c > '9')
    return;

  *input = input->substr(0, input->size() - left_offset_pos) + c +
    input->substr(input->size() - left_offset_pos);
}

void
ItemIntField::on_input_update()
{
  if (input->empty() || *input == "-")
  {
    *number = 0;
    return;
  }

  try
  {
    int new_number = std::stoi(*input);
    *number = new_number;
  }
  catch (...)
  {
    *input = std::to_string(*number);
  }
}

// Text manipulation and navigation functions

void
ItemIntField::insert_text(const std::string& text, const int left_offset_pos)
{
  update_undo();
  for (auto& c : text)
  {
    add_char(c, left_offset_pos);
  }
  on_input_update();
}

/* EOF */
