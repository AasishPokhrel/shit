//  SuperTux - Corrupted Granito - An "Evil" Granito
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

#ifndef HEADER_SUPERTUX_BADGUY_CORRUPTEDGRANITO_HPP
#define HEADER_SUPERTUX_BADGUY_CORRUPTEDGRANITO_HPP

#include "badguy/badguy.hpp"

/** An "Evil" Granito. */
class CorruptedGranito final : public BadGuy
{
public:
  enum Type {
    GRANITO, SKULLYHOP
  };

public:
  CorruptedGranito(const ReaderMapping& reader);
  CorruptedGranito(const ReaderMapping& reader, int type);

  virtual void initialize() override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual bool collision_squished(MovingObject& object) override;
  virtual void active_update(float dt_sec) override;

  virtual void unfreeze(bool melt = true) override;
  virtual bool is_freezable() const override;
  static std::string class_name() { return "corrupted_granito"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Corrupted Granito"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(CorruptedGranito)); }
  virtual bool is_snipable() const override { return true; }
  virtual bool is_flammable() const override { return m_type != GRANITO; }

  virtual GameObjectTypes get_types() const override;
  virtual std::string get_default_sprite_name() const override;

private:

  enum CorruptedGranitoState {
    STANDING,
    CHARGING,
    JUMPING
  };

private:
  void set_state(CorruptedGranitoState newState);

private:
  Timer recover_timer;
  CorruptedGranitoState state;

private:
  CorruptedGranito(const CorruptedGranito&) = delete;
  CorruptedGranito& operator=(const CorruptedGranito&) = delete;
};

#endif

/* EOF */
