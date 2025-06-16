//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_PLANT_HPP
#define HEADER_SUPERTUX_BADGUY_PLANT_HPP

#include "badguy/badguy.hpp"

class Plant final : public BadGuy
{
public:
  Plant(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual void active_update(float dt_sec) override;
  virtual void ignite() override;
  static std::string class_name() { return "plant"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Plant"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Plant)); }

protected:
  enum PlantState {
    PLANT_SLEEPING,
    PLANT_WAKING,
    PLANT_WALKING
  };

private:
  Timer timer;
  PlantState state;

private:
  Plant(const Plant&) = delete;
  Plant& operator=(const Plant&) = delete;
};

#endif

/* EOF */
