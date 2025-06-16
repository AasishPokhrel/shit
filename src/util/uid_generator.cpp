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

#include "util/uid_generator.hpp"

#include "util/log.hpp"

uint8_t UIDGenerator::s_magic_counter = 1;

UIDGenerator::UIDGenerator() :
  m_magic(s_magic_counter++),
  m_id_counter()
{
  if (s_magic_counter == 0)
  {
    s_magic_counter = 1;
  }
}

UID
UIDGenerator::next()
{
  m_id_counter += 1;

  if (m_id_counter > 0xffffff)
  {
    log_warning << "UIDGenerator overflow" << std::endl;
    m_id_counter = 0;
  }

  return UID((m_magic << 24) | m_id_counter);
}

/* EOF */
