//  SuperTux
//  Copyright (C) 2006 Ondrej Hosek <ondra.hosek@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_FALLING_COIN_HPP
#define HEADER_SUPERTUX_OBJECT_FALLING_COIN_HPP

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "supertux/physic.hpp"

class FallingCoin final : public GameObject
{
public:
  FallingCoin(const Vector& start_position, float x_vel);
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(FallingCoin)); }

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual bool is_saveable() const override {
    return false;
  }

private:
  Physic physic;
  Vector pos;
  SpritePtr sprite;
};

#endif

/* EOF */
