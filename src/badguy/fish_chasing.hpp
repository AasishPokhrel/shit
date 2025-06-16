//  SuperTux
//  Copyright (C) 2022 Daniel Ward <weluvgoatz@gmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_FISH_CHASING_HPP
#define HEADER_SUPERTUX_BADGUY_FISH_CHASING_HPP

#include "badguy/fish_swimming.hpp"

class FishChasing final : public FishSwimming
{
public:
  FishChasing(const ReaderMapping& reader);

  virtual void active_update(float dt_sec) override;

  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;

  static std::string class_name() { return "fish-chasing"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Chasing Fish"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual std::string get_overlay_size() const override { return "2x2"; }
  virtual GameObjectClasses get_class_types() const override { return FishSwimming::get_class_types().add(typeid(FishChasing)); }
  virtual ObjectSettings get_settings() override;

  std::string get_default_sprite_name() const override;

private:
  enum ChaseState {
    NORMAL,
    FOUND,
    CHASING,
    LOST
  };

  ChaseState m_chase_state;
  Timer m_realization_timer;
  Timer m_acceleration_timer;
  bool m_is_accelerating;
  float m_last_chase_velocity;
  float m_track_distance;
  float m_lost_distance;
  float m_chase_speed;

private:
  FishChasing(const FishChasing&) = delete;
  FishChasing& operator=(const FishChasing&) = delete;
};

#endif

/* EOF */
