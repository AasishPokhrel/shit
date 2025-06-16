//  DartTrap - Shoots a Dart at regular intervals
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

#ifndef HEADER_SUPERTUX_BADGUY_DARTTRAP_HPP
#define HEADER_SUPERTUX_BADGUY_DARTTRAP_HPP

#include "object/sticky_object.hpp"

/** Badguy "DartTrap" - Shoots a Dart at regular intervals */
class DartTrap final : public StickyBadguy
{
public:
  DartTrap(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void activate() override;
  virtual void active_update(float dt_sec) override;

  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  static std::string class_name() { return "darttrap"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Dart Trap"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return StickyBadguy::get_class_types().add(typeid(DartTrap)); }

  virtual ObjectSettings get_settings() override;
  virtual GameObjectTypes get_types() const override;
  virtual std::string get_default_sprite_name() const override;
  virtual void kill_fall() override;

  virtual void on_flip(float height) override;
  virtual void on_type_change(int old_type) override;

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

protected:
  enum State {
    IDLE, LOADING
  };

  enum Type {
    GRANITO, SKULL
  };

  void load();
  void fire();

private:
  bool m_enabled;
  float m_initial_delay;
  float m_fire_delay;
  int m_ammo; // ammo left (-1 means unlimited)
  std::string m_dart_sprite;

  State m_state;
  Timer m_fire_timer;

private:
  DartTrap(const DartTrap&) = delete;
  DartTrap& operator=(const DartTrap&) = delete;
};

#endif

/* EOF */
