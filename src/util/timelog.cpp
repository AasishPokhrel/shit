//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "util/timelog.hpp"

#include <iostream>

#include "util/log.hpp"

Timelog::Timelog() :
  m_last_ticks(0),
  m_last_component(nullptr)
{
}

void
Timelog::log(const char* component)
{
  Uint32 current_ticks = SDL_GetTicks();

  if (m_last_component != nullptr) {
    log_info << "Component '" << m_last_component <<  "' finished after "
             << (current_ticks - m_last_ticks) / 1000.0 << " seconds"
             << std::endl;
  }

  m_last_ticks = current_ticks;
  m_last_component = component;
}

/* EOF */
