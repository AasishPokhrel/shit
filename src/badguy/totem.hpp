//  SuperTux - "Totem" Badguy
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

#ifndef HEADER_SUPERTUX_BADGUY_TOTEM_HPP
#define HEADER_SUPERTUX_BADGUY_TOTEM_HPP

#include "badguy/badguy.hpp"

/** "Totem" Badguy - A variable-height stack of wooden blocks  */
class Totem final : public BadGuy
{
public:
  Totem(const ReaderMapping& reader);
  ~Totem() override;

  virtual void initialize() override;
  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;

  virtual bool updatePointers(const GameObject* from_object, GameObject* to_object);
  static std::string class_name() { return "totem"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Totem"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Totem)); }
  virtual bool is_snipable() const override { return true; }

protected:
  virtual bool collision_squished(MovingObject& object) override;
  virtual void kill_fall() override;

  void jump_on(Totem* target); /**< jump on target */
  void jump_off(); /**< jump off current base */

  void synchronize_with(Totem* baseTotem); /**< synchronize position and movement with baseTotem */

private:
  Totem* carrying; /**< Totem we are currently carrying (or 0) */
  Totem* carried_by; /**< Totem by which we are currently carried (or 0) */

private:
  Totem(const Totem&) = delete;
  Totem& operator=(const Totem&) = delete;
};

#endif

/* EOF */
