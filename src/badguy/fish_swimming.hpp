//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_FISH_SWIMMING_HPP
#define HEADER_SUPERTUX_BADGUY_FISH_SWIMMING_HPP

#include "badguy/badguy.hpp"

class FishSwimming : public BadGuy
{
public:
  FishSwimming(const ReaderMapping& reader);
  FishSwimming(const ReaderMapping& reader, const std::string& spritename);

  virtual void initialize() override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;
  virtual void active_update(float dt_sec) override;
  virtual void after_editor_set() override;

  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;
  virtual bool is_freezable() const override;
  static std::string class_name() { return "fish-swimming"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Swimming Fish"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual std::string get_overlay_size() const override { return "2x1"; }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(FishSwimming)); }
  virtual std::vector<Direction> get_allowed_directions() const override;
  virtual ObjectSettings get_settings() override;

  virtual GameObjectTypes get_types() const override;
  virtual std::string get_default_sprite_name() const override;

  void turn_around();
  void maintain_velocity_x(float goal_x_velocity);
  void maintain_velocity_y(float goal_y_velocity);

protected:
  enum Type {
    SNOW,
    FOREST,
    CORRUPTED,
  };

  enum FishYState {
    DISRUPTED,
    BALANCED
  };

  FishYState m_state;
  Timer m_beached_timer;
  Timer m_float_timer;
  float m_radius;

  bool is_frontal_hit(const CollisionHit& hit) const;
  void setup_velocity();

private:
  FishSwimming(const FishSwimming&) = delete;
  FishSwimming& operator=(const FishSwimming&) = delete;
};

#endif

/* EOF */
