//  SuperTux - Badguy "Igel"
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//                2023 MatusGuy
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

#ifndef HEADER_SUPERTUX_BADGUY_IGEL_HPP
#define HEADER_SUPERTUX_BADGUY_IGEL_HPP

#include "badguy/walking_badguy.hpp"

#include "supertux/timer.hpp"

class Igel final : public WalkingBadguy
{
public:
  Igel(const ReaderMapping& reader);

  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit &hit) override;
  virtual HitResponse collision_badguy(BadGuy &badguy, const CollisionHit &hit) override;
  virtual bool can_break() const override;
  virtual void run_dead_script() override;

  virtual std::string get_overlay_size() const override { return "2x1"; }
  static std::string class_name() { return "igel"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Igel"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(Igel)); }

  virtual bool is_freezable() const override { return true; }
  virtual void unfreeze(bool melt = true) override;

  virtual GameObjectTypes get_types() const override;
  virtual std::string get_default_sprite_name() const override;

private:
  enum Type { NORMAL, CORRUPTED };
  enum State { STATE_NORMAL, STATE_CHARGING, STATE_ROLLING };

  bool should_roll() const;
  void charge();
  void roll();
  void stop_rolling(bool bonk = false);
  float get_normal_walk_speed() const;

private:
  State m_state;
  Timer m_roll_timer;
  Timer m_roll_cooldown;
  Timer m_ease_timer;
  bool m_bonked;

private:
  Igel(const Igel&) = delete;
  Igel& operator=(const Igel&) = delete;
};

#endif

/* EOF */
