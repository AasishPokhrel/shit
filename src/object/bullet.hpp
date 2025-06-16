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

#ifndef HEADER_SUPERTUX_OBJECT_BULLET_HPP
#define HEADER_SUPERTUX_OBJECT_BULLET_HPP

#include "sprite/sprite_ptr.hpp"
#include "supertux/direction.hpp"
#include "supertux/moving_object.hpp"
#include "supertux/physic.hpp"
#include "supertux/player_status.hpp"
#include "video/layer.hpp"

class Player;

class Bullet final : public MovingObject
{
public:
  Bullet(const Vector& pos, const Vector& xm, Direction dir, BonusType type, Player& player);

  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(Bullet)); }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual bool is_saveable() const override { return false; }

  virtual int get_layer() const override { return LAYER_OBJECTS; }

  /** Makes bullet bounce off an object (that got hit). To be called
      by the collision handler of that object. Note that the @c hit
      parameter is filled in as perceived by the object, not by the
      bullet. */
  void ricochet(GameObject& other, const CollisionHit& hit);

  inline BonusType get_type() const { return type; }

  inline Player& get_player() const { return m_player; }

private:
  Player& m_player;
  Physic physic;
  int life_count;
  SpritePtr sprite;
  SpritePtr lightsprite;
  BonusType type;

private:
  Bullet(const Bullet&) = delete;
  Bullet& operator=(const Bullet&) = delete;
};

#endif

/* EOF */
