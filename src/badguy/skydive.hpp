//  SuperTux
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#ifndef HEADER_SUPERTUX_BADGUY_SKYDIVE_HPP
#define HEADER_SUPERTUX_BADGUY_SKYDIVE_HPP

#include "badguy/badguy.hpp"

class SkyDive final : public BadGuy
{
public:
  SkyDive(const ReaderMapping& reader);
  
  virtual void kill_fall() override;

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual void collision_tile(uint32_t tile_attributes) override;
  virtual void initialize() override;

  /* Inherited from Portable */
  virtual void grab(MovingObject& object, const Vector& pos, Direction dir) override;
  virtual void ungrab(MovingObject& object, Direction dir) override;

  virtual bool is_freezable() const override;

  virtual std::string get_overlay_size() const override { return "2x2"; }
  static std::string class_name() { return "skydive"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Skydive"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(SkyDive)); }
  virtual bool is_snipable() const override { return true; }

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual bool collision_squished(MovingObject& obj) override;

  void explode();
  virtual bool is_portable() const override;

private:
  SkyDive(const SkyDive&) = delete;
  SkyDive& operator=(const SkyDive&) = delete;
};

#endif

/* EOF */
