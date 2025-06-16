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

#ifndef HEADER_SUPERTUX_BADGUY_FLYINGSNOWBALL_HPP
#define HEADER_SUPERTUX_BADGUY_FLYINGSNOWBALL_HPP

#include "badguy/badguy.hpp"

class FlyingSnowBall final : public BadGuy
{
public:
  FlyingSnowBall(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void activate() override;
  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  static std::string class_name() { return "flyingsnowball"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Flying Snowball"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(FlyingSnowBall)); }
  virtual bool is_snipable() const override { return true; }

protected:
  virtual bool collision_squished(MovingObject& object) override;
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  float total_time_elapsed;
  Timer puff_timer; /**< time until the next smoke puff is spawned */

private:
  FlyingSnowBall(const FlyingSnowBall&) = delete;
  FlyingSnowBall& operator=(const FlyingSnowBall&) = delete;
};

#endif

/* EOF */
