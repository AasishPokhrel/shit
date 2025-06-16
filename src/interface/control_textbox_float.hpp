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

#ifndef HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTBOX_FLOAT_HPP
#define HEADER_SUPERTUX_INTERFACE_CONTROL_TEXTBOX_FLOAT_HPP

#include "interface/control_textbox.hpp"

class ControlTextboxFloat : public ControlTextbox
{
public:
  ControlTextboxFloat();

  virtual void update(float dt_sec) override;

  inline float get_value() const { return *m_value; }
  inline void set_value(float value) { *m_value = value; revert_value(); }
  /** Binds a float to this textbox. Set m_validate_float(float) if you want
   *  custom validation. (You may also use m_validate_string(string), though
   *  it's not recommended)
   *  @param value A pointer to the value to be bound. MUST NOT BE NULL (FIXME)
   */
  inline void bind_value(float* value) { m_value = value; revert_value(); }

protected:
  virtual bool parse_value(bool call_on_change = true) override;
  virtual void revert_value() override;

public:
  /** Optional, a function to validate the float. If nullptr, then all values
   *  are assumed to be valid.
   *
   *  @param ControlTextboxFloat* A pointer to the original control.
   *  @param float The float that is about to be applied to the textbox.
   *  @returns Whether or not this value is valid. If not, then the internal
   *           values will not be modified. Tip : You can manually change the
   *           values using set_value() inside this function and return false
   *           to establish a custom value (for example, a max float value).
   *  @see m_validate_string If you want to validate using a string instead.
   */
  bool (*m_validate_float)(ControlTextboxFloat*, float);

private:
  float* m_value;

private:
  ControlTextboxFloat(const ControlTextboxFloat&) = delete;
  ControlTextboxFloat& operator=(const ControlTextboxFloat&) = delete;
};

#endif

/* EOF */
