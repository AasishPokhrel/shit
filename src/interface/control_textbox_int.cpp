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

#include "interface/control_textbox_int.hpp"

#include <string>
#include <exception>

ControlTextboxInt::ControlTextboxInt() :
  m_validate_int(),
  m_value(nullptr)
{
  revert_value();
}

void
ControlTextboxInt::update(float dt_sec)
{
  ControlTextbox::update(dt_sec);
  if (!m_has_focus)
    revert_value();
}

bool
ControlTextboxInt::parse_value(bool call_on_change /* = true (see header */)
{
  if (!m_value)
    return false;

  // Calling super will put the correct value in m_string.
  if (!ControlTextbox::parse_value(false)) {
    // If the parent has failed, abandon. Keeping parsing should still result
    // in the parsing of a correct value (get_string() will return the last
    // valid value), but it would be unnecessary, since the last valid value
    // is already the one that's currently displayed.
    return false;
  }

  int temp;
  try {
    temp = std::stoi(get_contents());
  } catch (std::exception&) {
    revert_value();
    return false;
  }

  if (m_validate_int) {
    if (!m_validate_int(this, temp)) {
      revert_value();
      return false;
    }
  }

  if (*m_value != temp) {
    *m_value = temp;

    // Revert the value regardless.
    revert_value();

    if (call_on_change && m_on_change)
      m_on_change();
  }

  return true;
}

void
ControlTextboxInt::revert_value()
{
  if (!m_value)
    return;

  m_internal_string_backup = std::to_string(*m_value);
  ControlTextbox::revert_value();
}


/* EOF */
