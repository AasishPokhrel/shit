//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_PORTABLE_HPP
#define HEADER_SUPERTUX_OBJECT_PORTABLE_HPP

#include "supertux/direction.hpp"
#include "supertux/game_object_component.hpp"

class MovingObject;

/** An object that inherits from this object is considered "portable" and can
    be carried around by the player.
    The object has to additionally set the PORTABLE flag (this allows to
    make the object only temporarily portable by resetting the flag) */
class Portable : public GameObjectComponent
{
public:
  Portable() {}
  ~Portable() override {}

  /** called each frame when the object has been grabbed. */
  virtual void grab(MovingObject& other, const Vector& pos, Direction dir) { m_owner = &other; }
  virtual void ungrab(MovingObject& other, Direction ) { m_owner = nullptr; }

  inline MovingObject* get_owner() const { return m_owner; }
  virtual bool is_grabbed() const { return m_owner != nullptr; }
  virtual bool is_portable() const { return true; }

  /** Is the object so heavy/bulky/fragile that Tux can't run while
      carrying it? */
  virtual bool is_hampering() const { return false; }

protected:
  MovingObject* m_owner = nullptr;

private:
  Portable(const Portable&) = delete;
  Portable& operator=(const Portable&) = delete;
};

#endif

/* EOF */
