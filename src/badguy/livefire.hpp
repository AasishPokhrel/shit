//  SuperTux badguy - walking flame that glows
//  Copyright (C) 2013 LMH <lmh.0013@gmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_LIVEFIRE_HPP
#define HEADER_SUPERTUX_BADGUY_LIVEFIRE_HPP

#include "badguy/walking_badguy.hpp"

class LiveFire : public WalkingBadguy
{
public:
  LiveFire(const ReaderMapping& reader);

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual void active_update(float dt_sec) override;

  virtual void freeze() override;
  virtual bool is_freezable() const override;
  virtual bool is_flammable() const override;

  virtual void kill_fall() override;
  static std::string class_name() { return "livefire"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Walking Flame"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(LiveFire)); }

private:
  std::string death_sound;

protected:
  enum SState {
    STATE_SLEEPING,
    STATE_WAKING,
    STATE_WALKING,
    STATE_DORMANT,
    STATE_DEAD
  };

protected:
  SState state;

private:
  LiveFire(const LiveFire&) = delete;
  LiveFire& operator=(const LiveFire&) = delete;
};

class LiveFireAsleep final : public LiveFire
{
public:
  LiveFireAsleep(const ReaderMapping& reader);

  virtual void draw(DrawingContext& context) override;

  virtual void initialize() override;
  virtual void after_editor_set() override;
  static std::string class_name() { return "livefire_asleep"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Sleeping Flame"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return LiveFire::get_class_types().add(typeid(LiveFireAsleep)); }

private:
  LiveFireAsleep(const LiveFireAsleep&) = delete;
  LiveFireAsleep& operator=(const LiveFireAsleep&) = delete;
};

class LiveFireDormant final : public LiveFire
{
public:
  LiveFireDormant(const ReaderMapping& reader);

  virtual void draw(DrawingContext& context) override;

  virtual void initialize() override;
  virtual void after_editor_set() override;
  static std::string class_name() { return "livefire_dormant"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Dormant Flame"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return LiveFire::get_class_types().add(typeid(LiveFireDormant)); }

private:
  LiveFireDormant(const LiveFireDormant&) = delete;
  LiveFireDormant& operator=(const LiveFireDormant&) = delete;
};

#endif

/* EOF */
