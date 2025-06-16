//  SuperTux
//  Copyright (C) 2007 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_UTIL_OBSTACKPP_HPP
#define HEADER_SUPERTUX_UTIL_OBSTACKPP_HPP

#include <obstack.h>

inline void*
operator new (size_t bytes, struct obstack& obst)
{
  return obstack_alloc(&obst, static_cast<int>(bytes));
}

inline void*
operator new[] (size_t bytes, struct obstack& obst)
{
  return obstack_alloc(&obst, static_cast<int>(bytes));
}

inline void
operator delete (void* obj, struct obstack& obst)
{
  obstack_free(&obst, static_cast<char*>(obj));
}

inline void
operator delete[] (void* obj, struct obstack& obst)
{
  obstack_free(&obst, static_cast<char*>(obj));
}

static inline void* obstack_chunk_alloc(size_t size)
{
  return new char[size];
}

static inline void obstack_chunk_free(void* data)
{
  char* ptr = static_cast<char*>(data);
  delete[] ptr;
}

#endif

/* EOF */
