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

#ifndef HEADER_SUPERTUX_UTIL_UID_GENERATOR_HPP
#define HEADER_SUPERTUX_UTIL_UID_GENERATOR_HPP

#include "util/uid.hpp"

class UIDGenerator
{
private:
  static uint8_t s_magic_counter;

public:
  UIDGenerator();

  UID next();

private:
  uint8_t m_magic;
  uint32_t m_id_counter;

private:
  UIDGenerator(const UIDGenerator&) = delete;
  UIDGenerator& operator=(const UIDGenerator&) = delete;
};

#endif

/* EOF */
