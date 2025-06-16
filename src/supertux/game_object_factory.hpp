//  SuperTux
//  Copyright (C) 2004 Ricardo Cruz <rick2@aeiou.pt>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_FACTORY_HPP
#define HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_FACTORY_HPP

#include "supertux/object_factory.hpp"

#include "math/vector.hpp"

namespace ssq {
class VM;
} // namespace ssq

class GameObjectFactory final : public ObjectFactory
{
public:
  static GameObjectFactory& instance();

  /** Register all scriptable objects to a Squirrel VM. */
  static void register_objects(ssq::VM& vm);

public:
  using ObjectFactory::create;
  std::unique_ptr<GameObject> create(const std::string& name, const std::string& data) const;
  std::unique_ptr<GameObject> create(const std::string& name,
                                     const Vector& pos = {}, const Direction& dir = Direction::AUTO,
                                     const std::string& data = {}) const;

private:
  GameObjectFactory();

  void init_factories();

private:
  GameObjectFactory(const GameObjectFactory&) = delete;
  GameObjectFactory& operator=(const GameObjectFactory&) = delete;
};

#endif

/* EOF */
