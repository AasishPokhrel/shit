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

#ifndef HEADER_SUPERTUX_OBJECT_RAINSPLASH_HPP
#define HEADER_SUPERTUX_OBJECT_RAINSPLASH_HPP

#include "math/vector.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/game_object.hpp"

class Player;

class RainSplash final : public GameObject
{
public:
  RainSplash(const Vector& pos, bool vertical);
  ~RainSplash() override;
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(RainSplash)); }
  virtual bool is_saveable() const override {
    return false;
  }

protected:
  virtual void hit(Player& );
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

private:
  SpritePtr sprite;
  Vector position;
  int frame;
};

#endif

/* EOF */
