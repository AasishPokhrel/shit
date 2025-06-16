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

#include "util/fade_helper.hpp"

FadeHelper::FadeHelper(float time, float target_value,
                       float start_value, easing ease) :
  m_value(nullptr),
  m_progress(start_value),
  m_start(start_value),
  m_target(target_value),
  m_time(0.f),
  m_total_time(time),
  m_ease(ease)
{
}

FadeHelper::FadeHelper(float* value, float time,
                       float target_value, easing ease) :
  m_value(value),
  m_progress(*value),
  m_start(*value),
  m_target(target_value),
  m_time(0.f),
  m_total_time(time),
  m_ease(ease)
{
}

float
FadeHelper::update(float dt_sec)
{
  m_time += dt_sec;

  if (completed())
  {
    m_time = m_total_time;
    m_progress = m_target;
    if (m_value)
      *m_value = m_target;
    return m_target;
  }

  // FLOAT/DOUBLE CONVERSION
  // If at some point in development, floats are changed to doubles, or the
  // ease funciton return type change from double to float, remove the casts
  // here (it takes a lot of space).               ~ Semphris
  float progress = m_start + (m_target - m_start) * static_cast<float>(m_ease(
                                  static_cast<double>(m_time / m_total_time)));

  m_progress = progress;
  if (m_value)
    *m_value = progress;
  return progress;
}

bool
FadeHelper::completed() const
{
  return m_time >= m_total_time;
}

float
FadeHelper::get_value() const
{
  return m_progress;
}

/* EOF */
