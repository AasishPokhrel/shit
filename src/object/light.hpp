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

#ifndef HEADER_SUPERTUX_OBJECT_LIGHT_HPP
#define HEADER_SUPERTUX_OBJECT_LIGHT_HPP

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "video/color.hpp"

class Light : public GameObject
{
public:
  Light(const Vector& center, const Color& color = Color(1.0, 1.0, 1.0, 1.0));
  ~Light() override;
  virtual bool is_saveable() const override {
    return false;
  }

  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(Light)); }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

protected:
  Vector position;
  Color color;
  SpritePtr sprite;
};

#endif

/* EOF */
