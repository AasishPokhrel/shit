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

#ifndef HEADER_SUPERTUX_UTIL_UID_HPP
#define HEADER_SUPERTUX_UTIL_UID_HPP

#include <assert.h>
#include <stdint.h>
#include <functional>
#include <iosfwd>

class UID;

namespace std {

template<>
struct hash<UID>
{
  size_t operator()(const UID& uid) const;
};

} // namespace std {

class UID
{
  friend class UIDGenerator;
  friend std::ostream& operator<<(std::ostream& os, const UID& uid);
  friend size_t std::hash<UID>::operator()(const UID&) const;

public:
  using Magic = uint8_t;

private:
  explicit UID(uint32_t value) :
    m_value(value)
  {
    assert(m_value != 0);
  }

public:
  UID() : m_value(0) {}
  UID(const UID& other) = default;
  UID& operator=(const UID& other) = default;

  inline UID& operator=(uint32_t value) {
    m_value = value;
    return *this;
  }

  inline operator bool() const {
    return m_value != 0;
  }

  inline bool operator<(const UID& other) const {
    return m_value < other.m_value;
  }

  inline bool operator==(const UID& other) const {
    return m_value == other.m_value;
  }

  inline bool operator!=(const UID& other) const {
    return m_value != other.m_value;
  }

  inline Magic get_magic() const { return static_cast<Magic>((m_value & 0xffff0000u) >> 16); }

protected:
  uint32_t m_value;
};

std::ostream& operator<<(std::ostream& os, const UID& uid);

#endif

/* EOF */
