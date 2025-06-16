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

#ifndef HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_ITERATOR_HPP
#define HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_ITERATOR_HPP

#include <vector>

#include "game_object_manager.hpp"

template<typename T>
class GameObjectIterator final
{
public:
  typedef std::vector<GameObject* >::const_iterator Iterator;

public:
  GameObjectIterator(Iterator it, Iterator end) :
    m_it(it),
    m_end(end),
    m_object()
  {
    if (m_it != m_end)
    {
      // A dynamic_cast is needed to perform sidecasts (a.k.a. crosscasts)
      // T may be one of multiple base classes of the object and need not inherit GameObject
      if constexpr (std::is_base_of<GameObject, T>::value)
      {
        m_object = static_cast<T*>(*m_it);
      }
      else
      {
        m_object = dynamic_cast<T*>(*m_it);
        assert(m_object);
      }
    }
  }

  GameObjectIterator& operator++()
  {
    ++m_it;
    if (m_it != m_end)
    {
      if constexpr (std::is_base_of<GameObject, T>::value)
      {
        m_object = static_cast<T*>(*m_it);
      }
      else
      {
        m_object = dynamic_cast<T*>(*m_it);
        assert(m_object);
      }
    }
    return *this;
  }

  GameObjectIterator operator++(int)
  {
    GameObjectIterator tmp(*this);
    operator++();
    return tmp;
  }

  inline T* get() const { return m_object; }

  inline T* operator->() {
    return m_object;
  }

  inline const T* operator->() const {
    return m_object;
  }

  inline T& operator*() const {
    return *m_object;
  }

  inline T& operator*() {
    return *m_object;
  }

  inline bool operator==(const GameObjectIterator& other) const
  {
    return m_it == other.m_it;
  }

  inline bool operator!=(const GameObjectIterator& other) const
  {
    return !(*this == other);
  }

private:
  Iterator m_it;
  Iterator m_end;
  T* m_object;
};

template<typename T>
class GameObjectRange final
{
public:
  GameObjectRange(const GameObjectManager& manager) :
    m_manager(manager)
  {}

  GameObjectIterator<T> begin() const {
    auto& objects = m_manager.get_objects_by_type_index(typeid(T));
    return GameObjectIterator<T>(objects.begin(), objects.end());
  }

  GameObjectIterator<T> end() const {
    auto& objects = m_manager.get_objects_by_type_index(typeid(T));
    return GameObjectIterator<T>(objects.end(), objects.end());
  }

private:
  const GameObjectManager& m_manager;
};

#endif

/* EOF */
