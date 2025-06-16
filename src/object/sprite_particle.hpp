//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_SPRITE_PARTICLE_HPP
#define HEADER_SUPERTUX_OBJECT_SPRITE_PARTICLE_HPP

#include "math/anchor_point.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "video/color.hpp"
#include "video/drawing_context.hpp"

class Player;

class SpriteParticle final : public GameObject
{
public:
  SpriteParticle(SpritePtr sprite, const std::string& action,
                 const Vector& position, AnchorPoint anchor,
                 const Vector& velocity, const Vector& acceleration,
                 int drawing_layer = LAYER_OBJECTS-1, bool notimeout = false, Color color = Color::WHITE);
  SpriteParticle(const std::string& sprite_name, const std::string& action,
                 const Vector& position, AnchorPoint anchor,
                 const Vector& velocity, const Vector& acceleration,
                 int drawing_layer = LAYER_OBJECTS-1, bool notimeout = false, Color color = Color::WHITE);
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(SpriteParticle)); }
  ~SpriteParticle() override;

protected:
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual bool is_saveable() const override {
    return false;
  }

private:
  SpritePtr sprite;
  Vector position;
  Vector velocity;
  Vector acceleration;
  int drawing_layer;
  SpritePtr lightsprite;
  bool glow;
  bool no_time_out;
  Color color;

private:
  SpriteParticle(const SpriteParticle&) = delete;
  SpriteParticle& operator=(const SpriteParticle&) = delete;
};

#endif

/* EOF */
