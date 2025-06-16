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

#ifndef HEADER_SUPERTUX_OBJECT_FLOWER_HPP
#define HEADER_SUPERTUX_OBJECT_FLOWER_HPP

#include "supertux/moving_object.hpp"

#include "sprite/sprite_ptr.hpp"
#include "supertux/player_status.hpp"
#include "video/flip.hpp"
#include "video/layer.hpp"

class Flower final : public MovingObject
{
  friend class FlipLevelTransformer;

public:
  Flower(BonusType type, const std::string& custom_sprite = "");
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(Flower)); }

  virtual bool is_saveable() const override { return false; }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  virtual void on_flip(float height) override;

  virtual int get_layer() const override { return LAYER_OBJECTS; }

private:
  BonusType type;
  SpritePtr sprite;
  Flip flip;

  SpritePtr lightsprite;

private:
  Flower(const Flower&) = delete;
  Flower& operator=(const Flower&) = delete;
};

#endif

/* EOF */
