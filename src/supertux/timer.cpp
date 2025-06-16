//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include <math.h>

#include "supertux/timer.hpp"

Timer::Timer() :
  m_period(0),
  m_cycle_start(0),
  m_cycle_pause(0),
  m_cyclic(false)
{
}

void
Timer::start(float period, bool cyclic)
{
  m_period = period;
  m_cyclic = cyclic;
  m_cycle_start = g_game_time;
  m_cycle_pause = 0;
}

bool
Timer::check()
{
  if (m_period == 0)
    return false;

  if (g_game_time - m_cycle_start >= m_period) {
    if (m_cyclic) {
      m_cycle_start = g_game_time - fmodf(g_game_time - m_cycle_start, m_period);
    } else {
      m_period = 0;
    }
    return true;
  }

  return false;
}

void
Timer::pause()
{
  float left = get_timeleft();
  stop();
  m_cycle_pause = left;
}

void
Timer::resume()
{
  start(m_cycle_pause);
}

/* EOF */
