//  SuperTux
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

#ifndef HEADER_SUPERTUX_BADGUY_GRANITO_GIANT_HPP
#define HEADER_SUPERTUX_BADGUY_GRANITO_GIANT_HPP

#include "badguy/badguy.hpp"

class GranitoGiant final : public BadGuy
{
public:
  explicit GranitoGiant(const ReaderMapping& reader);

  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual void initialize() override;

  static std::string class_name() { return "granito_giant"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Giant Granito"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(GranitoGiant)); }

  virtual void kill_fall() override;

  virtual bool is_flammable() const override { return false; }
  virtual bool is_freezable() const override { return false; }
  virtual bool is_hurtable() const override { return false; }
  virtual bool is_snipable() const override { return false; }

  GameObjectTypes get_types() const override;
  virtual std::string get_default_sprite_name() const override;
  virtual void after_editor_set() override;

private:
  enum Type { AWAKE, SLEEP, CORRUPTED_A, CORRUPTED_B, CORRUPTED_C };

private:
  GranitoGiant(const GranitoGiant&) = delete;
  GranitoGiant& operator=(const GranitoGiant&) = delete;
};

#endif

/* EOF */
