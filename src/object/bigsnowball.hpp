//  Copyright (C) 2024 Daniel Ward <weluvgoatz@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_BIGSNOWBALL_HPP
#define HEADER_SUPERTUX_OBJECT_BIGSNOWBALL_HPP

#include "object/moving_sprite.hpp"

#include "supertux/physic.hpp"

enum class Direction;

class BigSnowball final : public MovingSprite
{
public:
  BigSnowball(const ReaderMapping& reader);
  BigSnowball(const Vector& pos, const Direction& dir, bool bounce = false);

  virtual ObjectSettings get_settings() override;

  virtual void update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  static std::string class_name() { return "bigsnowball"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Big Snowball"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(BigSnowball)); }

private:
  void spawn_particles();

private:
  Physic m_physic;
  Direction m_dir;
  float m_speed;
  bool m_break_on_impact;
  bool m_bounce;

private:
  BigSnowball(const BigSnowball&) = delete;
  BigSnowball& operator=(const BigSnowball&) = delete;
};

#endif

/* EOF */
