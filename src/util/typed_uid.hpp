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

#ifndef HEADER_SUPERTUX_UTIL_TYPED_UID_HPP
#define HEADER_SUPERTUX_UTIL_TYPED_UID_HPP

#include "util/uid.hpp"

#include "supertux/sector.hpp"

/**
 * Typed UIDs allows storing GameObjects as UID while offering to option to use
 * the object either as if it was a pointer or as if it was an UID. This helps
 * staying resilient against the objects' variable lifetime.
 */
template<class T>
class TypedUID : public UID
{
public:
  TypedUID() : UID() {}
  TypedUID(const T* object) : UID() { if (object) *this = object->get_uid(); }
  TypedUID(const TypedUID& other) = default;
  TypedUID& operator=(const TypedUID& other) = default;

  inline T* get() const
  {
    if (m_value == 0 || !Sector::current())
      return nullptr;

    auto* object = Sector::get().get_object_by_uid<T>(*this);

    // Would break const correctness
#if 0
    if (!object)
    {
      *this = UID(); 
    }
#endif

    return object;
  }

  inline TypedUID& operator=(const T* object)
  {
    *this = object ? object->get_uid() : UID();
    return *this;
  }

  inline TypedUID& operator=(const UID& other)
  {
    UID::operator=(other);
    return *this;
  }

  inline T* operator->() const
  {
    return get();
  }

  inline T& operator*() const
  {
    T* t = get();
    if (!t)
      throw std::runtime_error("Attempt to dereference invalid TypedUID");

    return *t;
  }

  inline bool operator==(const T* object) const
  {
    return (!object && m_value == 0) || object->get_uid() == *this;
  }

  inline bool operator!=(const T* object) const
  {
    return object ? object->get_uid() != *this : m_value == 0;
  }

  inline operator bool() const {
    return m_value != 0 && Sector::current() && Sector::get().get_object_by_uid<T>(*this);
  }
};

template<class T>
inline bool operator==(const T* object, const TypedUID<T>& typed_uid) {
  return typed_uid == object;
}

template<class T>
inline bool operator!=(const T* object, const TypedUID<T>& typed_uid) {
  return typed_uid != object;
}

#endif

/* EOF */
