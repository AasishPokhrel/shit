//  SuperTux - Crystallo
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

#ifndef HEADER_SUPERTUX_BADGUY_CRYSTALLO_HPP
#define HEADER_SUPERTUX_BADGUY_CRYSTALLO_HPP

#include "badguy/walking_badguy.hpp"

/** Basic badguy, patrols around a fixed position. */
class Crystallo final : public WalkingBadguy
{
public:
  Crystallo(const ReaderMapping& reader);
  Crystallo(const Vector& pos, const Vector& start_pos, float vel_x, SpritePtr sprite,
            Direction dir, float radius, const std::string& script);

  virtual ObjectSettings get_settings() override;
  static std::string class_name() { return "crystallo"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Crystallo"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(Crystallo)); }

  virtual void active_update(float dt_sec) override;
  virtual bool is_flammable() const override;
  virtual bool is_snipable() const override { return true; }

  virtual void on_flip(float height) override;

protected:
  virtual bool collision_squished(MovingObject& object) override;

private:
  float m_radius;

private:
  Crystallo(const Crystallo&) = delete;
  Crystallo& operator=(const Crystallo&) = delete;
};

#endif

/* EOF */
