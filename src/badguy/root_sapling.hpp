//  SuperTux - Root Sapling Badguy
//  Copyright (C) 2023 MatusGuy
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

#ifndef HEADER_SUPERTUX_BADGUY_ROOT_SAPLING_HPP
#define HEADER_SUPERTUX_BADGUY_ROOT_SAPLING_HPP

#include "badguy/badguy.hpp"

#include "collision/collision_system.hpp"

class RootSapling final : public BadGuy
{
public:
  RootSapling(const ReaderMapping& reader);

  virtual void kill_fall() override;
  virtual HitResponse collision_badguy(BadGuy& other, const CollisionHit& hit) override;
  virtual bool collision_squished(MovingObject& object) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual void active_update(float) override;

  virtual bool is_flammable() const override { return false; }
  virtual bool is_freezable() const override { return false; }
  virtual bool is_snipable() const override { return false; }

  static std::string class_name() { return "root_sapling"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Root Sapling"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(RootSapling)); }

  virtual void on_flip(float height) override;

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  void summon_root();
  bool should_summon_root(const Rectf& bbox);

  /** Fix raycasting down/right. More info in the implementation. */
  CollisionSystem::RaycastResult reverse_raycast(const Vector& line_start, const Vector& line_end);

private:
  Timer m_root_timer;
  bool m_dead;

private:
  RootSapling(const RootSapling&) = delete;
  RootSapling& operator=(const RootSapling&) = delete;
};

#endif

/* EOF */
