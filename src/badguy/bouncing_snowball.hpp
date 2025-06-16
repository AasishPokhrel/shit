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

#ifndef HEADER_SUPERTUX_BADGUY_BOUNCING_SNOWBALL_HPP
#define HEADER_SUPERTUX_BADGUY_BOUNCING_SNOWBALL_HPP

#include "badguy/badguy.hpp"

class BouncingSnowball final : public BadGuy
{
public:
  BouncingSnowball(const ReaderMapping& reader);
  BouncingSnowball(const Vector& pos, Direction d, float x_vel = 80);

  virtual void initialize() override;

  virtual void active_update(float) override;

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;

  virtual void unfreeze(bool melt = true) override;

  virtual std::string get_overlay_size() const override { return "2x2"; }
  static std::string class_name() { return "bouncingsnowball"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Bouncing Snowball"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(BouncingSnowball)); }

  virtual GameObjectTypes get_types() const override;
  virtual std::string get_default_sprite_name() const override;

  virtual void after_editor_set() override;
  virtual bool is_snipable() const override { return true; }
  virtual bool is_freezable() const override;

protected:
  virtual bool collision_squished(MovingObject& object) override;
  void turn_around();

private:
  enum Type {
    NORMAL,
    FATBAT
  };

  float m_x_speed;
  Timer m_turn_around_timer;
  int m_turn_around_counter; /**< Counts number of turns since turn_around_timer was started */


private:
  BouncingSnowball(const BouncingSnowball&) = delete;
  BouncingSnowball& operator=(const BouncingSnowball&) = delete;
};

#endif

/* EOF */
