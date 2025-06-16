//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_UTIL_TIMELOG_HPP
#define HEADER_SUPERTUX_UTIL_TIMELOG_HPP

#include <SDL.h>

class Timelog
{
public:
  Timelog();

  void log(const char* component = nullptr);

private:
  Uint32 m_last_ticks;
  const char* m_last_component = nullptr;

private:
  Timelog(const Timelog&) = delete;
  Timelog& operator=(const Timelog&) = delete;
};

#endif

/* EOF */
