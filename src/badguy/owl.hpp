//  SuperTux
//  Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#ifndef HEADER_SUPERTUX_BADGUY_OWL_HPP
#define HEADER_SUPERTUX_BADGUY_OWL_HPP

#include "badguy/badguy.hpp"

class Portable;

class Owl final : public BadGuy
{
private:
  static std::vector<std::string> s_portable_objects;

public:
  Owl(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual void kill_fall() override;

  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;
  virtual bool is_freezable() const override;
  virtual void ignite() override;

  virtual std::string get_overlay_size() const override { return "2x2"; }
  static std::string class_name() { return "owl"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Owl"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Owl)); }

  virtual ObjectSettings get_settings() override;
  virtual bool is_snipable() const override { return true; }

protected:
  bool is_above_player() const;
  void ungrab_carried_object();
  virtual void active_update (float dt_sec) override;
  virtual bool collision_squished(MovingObject& object) override;

  std::string carried_obj_name;
  Portable *carried_object;

private:
  Owl(const Owl&) = delete;
  Owl& operator=(const Owl&) = delete;
};

#endif

/* EOF */
