/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_SUPERTUX_UTIL_CURRENTON_HPP
#define HEADER_SUPERTUX_UTIL_CURRENTON_HPP

/**
 *   A 'Currenton' allows access to the currently active instance of a
 *   class via the static current() function. It is kind of like a
 *   singleton, but without handling the object construction itself or
 *   in other words its a glorified global variable that points to the
 *   current instance of a class.
 */
template<class C>
class Currenton
{
private:
  static Currenton<C>* s_current;

protected:
  Currenton()
  {
    // FIXME: temporarly disabled, as Sector() for the main menu,
    // doesn't get cleaned up before a real Sector() starts
    // assert(!s_current);
    s_current = this;
  }

  virtual ~Currenton()
  {
    if (s_current == this)
    {
      s_current = nullptr;
    }
  }

public:
  static C* current() { return static_cast<C*>(s_current); }
};

template<class C>
Currenton<C>* Currenton<C>::s_current = nullptr;

#endif

/* EOF */
