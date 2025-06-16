//  MoleRock - Rock thrown by "Mole" Badguy
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

#ifndef HEADER_SUPERTUX_BADGUY_MOLE_ROCK_HPP
#define HEADER_SUPERTUX_BADGUY_MOLE_ROCK_HPP

#include "badguy/badguy.hpp"

/** Badguy "MoleRock" - Rock thrown by "Mole" Badguy */
class MoleRock final : public BadGuy
{
public:
  MoleRock(const ReaderMapping& reader);
  MoleRock(const Vector& pos, const Vector& velocity, const BadGuy* parent);

  virtual void initialize() override;
  virtual void deactivate() override;

  virtual void active_update(float dt_sec) override;

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;

  virtual bool updatePointers(const GameObject* from_object, GameObject* to_object);

  virtual bool is_flammable() const override;
  virtual bool is_hurtable() const override { return false; }
  static std::string class_name() { return "mole_rock"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Mole's rock"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(MoleRock)); }

protected:
  const BadGuy* parent; /**< collisions with this BadGuy will be ignored */
  const Vector initial_velocity; /**< velocity at time of creation */

private:
  MoleRock(const MoleRock&) = delete;
  MoleRock& operator=(const MoleRock&) = delete;
};

#endif

/* EOF */
