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

#ifndef HEADER_SUPERTUX_OBJECT_ONEUP_HPP
#define HEADER_SUPERTUX_OBJECT_ONEUP_HPP

#include "object/moving_sprite.hpp"
#include "supertux/direction.hpp"
#include "supertux/physic.hpp"

class OneUp final : public MovingSprite
{
public:
  OneUp(const Vector& pos, Direction direction = Direction::RIGHT);
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(OneUp)); }
  virtual bool is_saveable() const override {
    return false;
  }

  virtual void update(float dt_sec) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

private:
  Physic physic;
};

#endif

/* EOF */
