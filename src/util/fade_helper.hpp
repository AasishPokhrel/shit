//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_UTIL_FADE_HELPER_HPP
#define HEADER_SUPERTUX_UTIL_FADE_HELPER_HPP

#include "math/easing.hpp"

class FadeHelper
{
public:
  /** Initialize FadeHelper without binding to a value */
  FadeHelper(float time, float target_value,
             float start_value = 0.f, easing ease = LinearInterpolation);

  /** Initialize FadeHelper, binding to @c value */
  FadeHelper(float* value, float time,
             float target_value, easing ease = LinearInterpolation);


  /**
   * Increases the internal timer of the FadeHelper.
   * 
   * @param dt_sec The increase in time since last call to update().
   * @returns The new value for the internal value.
   */
  float update(float dt_sec);

  /** @returns true if the FadeHelper has completed fading. */
  bool completed() const;

  float get_value() const;

private:
  float* m_value;
  float m_progress;
  float m_start, m_target,
        m_time, m_total_time;
  easing m_ease;

private:
  FadeHelper(const FadeHelper&) = delete;
  FadeHelper& operator=(const FadeHelper&) = delete;
};

#endif

/* EOF */
