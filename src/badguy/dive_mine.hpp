//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#ifndef HEADER_SUPERTUX_BADGUY_DIVEMINE_HPP
#define HEADER_SUPERTUX_BADGUY_DIVEMINE_HPP

#include "badguy/badguy.hpp"

#include "sprite/sprite_ptr.hpp"

class DiveMine final : public BadGuy
{
private:
  static const float s_trigger_radius;
  static const float s_swim_speed;
  static const float s_max_float_acceleration;

public:
  DiveMine(const ReaderMapping& reader);

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;

  virtual void kill_fall() override;

  virtual void draw(DrawingContext& context) override;
  virtual void active_update(float dt_sec) override;

  virtual void ignite() override;
  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;
  virtual bool is_freezable() const override { return true; }

  static std::string class_name() { return "dive-mine"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Dive Mine"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(DiveMine)); }

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  void reset_sprites();
  void stop_chasing();

  void explode();
  void turn_around();

private:
  SpritePtr m_ticking_glow;

  bool m_chasing;

private:
  DiveMine(const DiveMine&) = delete;
  DiveMine& operator=(const DiveMine&) = delete;
};

#endif

/* EOF */
