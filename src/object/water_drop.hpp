//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_WATER_DROP_HPP
#define HEADER_SUPERTUX_OBJECT_WATER_DROP_HPP

#include "object/moving_sprite.hpp"
#include "supertux/physic.hpp"

/** When a badguy melts, it creates this object. */

class WaterDrop final : public MovingSprite
{
public:
  WaterDrop(const Vector& pos, const std::string& sprite_path_, const Vector& velocity);

  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(WaterDrop)); }

  virtual void update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& ) override;

private:
  Physic physic;

  typedef enum {
    WDS_FALLING,
    WDS_SPLASH,
    WDS_PUDDLE
  } WaterDropState;

  WaterDropState wd_state;

  std::string sprite_path;
};

#endif

/* EOF */
