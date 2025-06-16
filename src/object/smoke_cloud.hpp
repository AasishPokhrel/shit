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

#ifndef HEADER_SUPERTUX_OBJECT_SMOKE_CLOUD_HPP
#define HEADER_SUPERTUX_OBJECT_SMOKE_CLOUD_HPP

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "supertux/timer.hpp"

class SmokeCloud final : public GameObject
{
public:
  SmokeCloud(const Vector& pos);
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(SmokeCloud)); }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual bool is_saveable() const override {
    return false;
  }

private:
  SpritePtr sprite;
  Timer timer;
  Vector position;

private:
  SmokeCloud(const SmokeCloud&) = delete;
  SmokeCloud& operator=(const SmokeCloud&) = delete;
};

#endif

/* EOF */
