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

#ifndef HEADER_SUPERTUX_BADGUY_RCRYSTALLO_HPP
#define HEADER_SUPERTUX_BADGUY_RCRYSTALLO_HPP

#include "badguy/walking_badguy.hpp"

class RCrystallo final : public WalkingBadguy
{
public:
  RCrystallo(const ReaderMapping& reader);
  RCrystallo(const Vector& pos, const Vector& start_pos, float vel_x, SpritePtr sprite,
             Direction dir, float radius, const std::string& script, bool fall = false);

  virtual void initialize() override;
  virtual ObjectSettings get_settings() override;
  static std::string class_name() { return "rcrystallo"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Roof Crystallo"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(RCrystallo)); }

  virtual void active_update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual bool is_flammable() const override;
  virtual void kill_fall() override;

  virtual void after_editor_set() override;
  virtual void on_flip(float height) override;

protected:
  enum RCrystalloState
  {
    RCRYSTALLO_ROOF,
    RCRYSTALLO_DETECT,
    RCRYSTALLO_FALLING
  };
  RCrystalloState m_state;

private:
  float m_radius;

private:
  RCrystallo(const RCrystallo&) = delete;
  RCrystallo& operator=(const RCrystallo&) = delete;
};

#endif

/* EOF */
