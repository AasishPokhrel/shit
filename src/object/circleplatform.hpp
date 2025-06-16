//  Copyright (C) 2020 Daniel Ward <weluvgoatz@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_CIRCLEPLATFORM_HPP
#define HEADER_SUPERTUX_OBJECT_CIRCLEPLATFORM_HPP

#include "object/moving_sprite.hpp"
#include "supertux/timer.hpp"

class CirclePlatform : public MovingSprite
{
public:
  CirclePlatform(const ReaderMapping& reader);

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual ObjectSettings get_settings() override;
  virtual void update(float dt_sec) override;
  virtual void on_flip(float height) override;
  virtual void draw(DrawingContext& context) override;
  
  static std::string class_name() { return "circleplatform"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Circular Platform"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(CirclePlatform)); }

private:
  virtual void initialize();

protected:
  Vector start_position;
  float angle;
  float radius;
  float speed;
  
  Timer timer;
  float time;

  SurfacePtr m_radius_indicator;

private:
  CirclePlatform(const CirclePlatform&) = delete;
  CirclePlatform& operator=(const CirclePlatform&) = delete;
};

#endif

/* EOF */
