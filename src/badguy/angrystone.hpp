//  AngryStone - A spiked block that charges towards the player
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_ANGRYSTONE_HPP
#define HEADER_SUPERTUX_BADGUY_ANGRYSTONE_HPP

#include "badguy/badguy.hpp"

class AngryStone final : public BadGuy
{
public:
  AngryStone(const ReaderMapping& reader);

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual void active_update(float dt_sec) override;
  virtual void kill_fall() override;
  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;
  virtual bool is_freezable() const override;
  virtual bool is_flammable() const override;

  virtual std::string get_overlay_size() const override { return "3x3"; }
  static std::string class_name() { return "angrystone"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Angry Stone"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(AngryStone)); }

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

protected:
  enum AngryStoneState
  {
    IDLE,
    CHARGING,
    ATTACKING,
    RECOVERING
  };

private:
  Vector m_attack_direction;  /**< A normalized vector representing the current attack direction. */
  Vector m_old_wall_direction; /**< If a wall was hit during the last attack, a normalized vector representing the direction of the last attack, (0,0) otherwise. */
  Timer m_timer;
  AngryStoneState m_state;

private:
  AngryStone(const AngryStone&) = delete;
  AngryStone& operator=(const AngryStone&) = delete;
};

#endif

/* EOF */
