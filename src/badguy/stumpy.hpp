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

#ifndef HEADER_SUPERTUX_BADGUY_STUMPY_HPP
#define HEADER_SUPERTUX_BADGUY_STUMPY_HPP

#include "badguy/walking_badguy.hpp"

class Stumpy final : public WalkingBadguy
{
public:
  Stumpy(const ReaderMapping& reader);
  Stumpy(const Vector& pos, Direction d);

  virtual void initialize() override;
  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;

  virtual bool is_freezable() const override;

  virtual std::string get_overlay_size() const override { return "2x2"; }
  static std::string class_name() { return "stumpy"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Stumpy"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(Stumpy)); }

protected:
  enum MyState {
    STATE_INVINCIBLE, STATE_NORMAL
  };

protected:
  virtual bool collision_squished(MovingObject& object) override;

private:
  MyState mystate;
  Timer   invincible_timer;

private:
  Stumpy(const Stumpy&) = delete;
  Stumpy& operator=(const Stumpy&) = delete;
};

#endif

/* EOF */
