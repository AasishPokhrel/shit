//  SuperTux
//  Copyright (C) 2021 Daniel Ward <weluvgoatz@gmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_SCRYSTALLO_HPP
#define HEADER_SUPERTUX_BADGUY_SCRYSTALLO_HPP

#include "badguy/walking_badguy.hpp"

class SCrystallo final : public WalkingBadguy
{
public:
  SCrystallo(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual ObjectSettings get_settings() override;
  static std::string class_name() { return "scrystallo"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Sleeping Crystallo"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(SCrystallo)); }

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;

  virtual void active_update(float dt_sec) override;
  virtual bool is_flammable() const override;
  virtual bool is_snipable() const override { return true; }

  virtual void after_editor_set() override;
  virtual void on_flip(float height) override;

protected:
  virtual bool collision_squished(MovingObject& object) override;
protected:
  enum SCrystalloState
  {
    SCRYSTALLO_SLEEPING,
    SCRYSTALLO_WAKING,
    SCRYSTALLO_JUMPING,
    SCRYSTALLO_WALKING
  };
  SCrystalloState m_state;

private:
  bool m_roof;
  float m_radius;
  float m_range;
  Vector m_radius_anchor;

private:
  SCrystallo(const SCrystallo&) = delete;
  SCrystallo& operator=(const SCrystallo&) = delete;
};

#endif
