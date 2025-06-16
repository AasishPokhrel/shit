//  SuperTux
//  Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_KAMIKAZESNOWBALL_HPP
#define HEADER_SUPERTUX_BADGUY_KAMIKAZESNOWBALL_HPP

#include "badguy/badguy.hpp"

/** Kamikaze Snowball will fly in one direction until he hits something.
    On impact he is destroyed, trying to kill what he hit or hit him. */
class KamikazeSnowball : public BadGuy
{
public:
  KamikazeSnowball(const ReaderMapping& reader,
                   const std::string& sprite_name = "images/creatures/snowball/kamikaze-snowball.sprite");

  virtual void initialize() override;
  virtual void collision_solid(const CollisionHit& hit) override;
  static std::string class_name() { return "kamikazesnowball"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Kamikaze Snowball"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(KamikazeSnowball)); }
  virtual bool is_snipable() const override { return true; }

protected:
  virtual bool collision_squished(MovingObject& object) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual void kill_collision();

private:
  KamikazeSnowball(const KamikazeSnowball&) = delete;
  KamikazeSnowball& operator=(const KamikazeSnowball&) = delete;
};

class LeafShot final : public KamikazeSnowball
{
public:
  LeafShot(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual bool is_freezable() const override;

  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;
  virtual void kill_collision() override;

  virtual std::string get_overlay_size() const override { return "2x1"; }

  static std::string class_name() { return "leafshot"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Leafshot"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return KamikazeSnowball::get_class_types().add(typeid(LeafShot)); }

  virtual bool is_snipable() const override { return true; }

  GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

protected:
  virtual bool collision_squished(MovingObject& object) override;

private:
  enum Type {
    NORMAL,
    CORRUPTED
  };

private:
  LeafShot(const LeafShot&) = delete;
  LeafShot& operator=(const LeafShot&) = delete;
};

#endif

/* EOF */
