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

#ifndef HEADER_SUPERTUX_UTIL_DYNAMIC_SCOPED_REF_HPP
#define HEADER_SUPERTUX_UTIL_DYNAMIC_SCOPED_REF_HPP

#include <assert.h>

template<typename T> class DynamicScopedRefGuard;

/** The DynamicScopedRef class emulates dynamic scoping in C++ by
    providing a rebindable reference to a value. Binding a new value
    to the DynamicScopedRef pushes it's previous value on the stack
    and restores it again once the guard object goes out of scope.
    Thus it provides a safer alternative to normal global variables as
    functions deeper down the stack can't manipulate the value
    persistantly. */
template<typename T>
class DynamicScopedRef
{
  friend class DynamicScopedRefGuard<T>;

public:
  DynamicScopedRef() :
    m_ptr(nullptr)
  {}

  T* get() const {
    return m_ptr;
  }

  T* operator->() const {
    assert(m_ptr != nullptr);
    return m_ptr;
  }

  T& operator*() const {
    assert(m_ptr != nullptr);
    return *m_ptr;
  }

  /** Set the DynamicScopedRef to a new value, the lifetime of the
      returned guard object decides when the ref gets reset to it's
      previous value */
  DynamicScopedRefGuard<T> bind(T& value)
  {
    return DynamicScopedRefGuard<T>(*this, &value);
  }

  /** Check if the ref contains a valid value */
  explicit operator bool() const {
    return m_ptr != nullptr;
  }

private:
  T* getset(T* ptr)
  {
    T* tmp = m_ptr;
    m_ptr = ptr;
    return tmp;
  }

private:
  T* m_ptr;

private:
  DynamicScopedRef(const DynamicScopedRef&) = delete;
  DynamicScopedRef& operator=(const DynamicScopedRef&) = delete;
};

/** The DynamicScopedRefGuard class is returned by
    DynamicScopedRef::bind() and ensures that the DynamicScopedRef
    gets reset to it's previous value once the guard goes out of
    scope. */
template<typename T>
class DynamicScopedRefGuard
{
public:
  DynamicScopedRefGuard(DynamicScopedRef<T>& dynamic_scoped, T* ptr) :
    m_dynamic_scoped(dynamic_scoped),
    m_old_ptr(m_dynamic_scoped.getset(ptr))
  {
  }

  DynamicScopedRefGuard(DynamicScopedRefGuard<T>&&) noexcept = default;

  ~DynamicScopedRefGuard()
  {
    m_dynamic_scoped.getset(m_old_ptr);
  }

private:
  DynamicScopedRef<T>& m_dynamic_scoped;
  T* m_old_ptr;

private:
  DynamicScopedRefGuard(const DynamicScopedRefGuard&) = delete;
  DynamicScopedRefGuard& operator=(const DynamicScopedRefGuard&) = delete;
};

#endif

/* EOF */
