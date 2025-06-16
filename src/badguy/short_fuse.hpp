//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_SHORT_FUSE_HPP
#define HEADER_SUPERTUX_BADGUY_SHORT_FUSE_HPP

#include "badguy/walking_badguy.hpp"

class ShortFuse final : public WalkingBadguy
{
public:
  ShortFuse(const ReaderMapping& reader);

  static std::string class_name() { return "short_fuse"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Short Fuse"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(ShortFuse)); }

protected:
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual bool collision_squished(MovingObject& object) override;
  virtual bool is_freezable() const override;
  virtual void freeze() override;
  virtual void kill_fall() override;
  virtual void ignite() override;
  virtual bool is_snipable() const override { return true; }

  void explode();

private:
  ShortFuse(const ShortFuse&) = delete;
  ShortFuse& operator=(const ShortFuse&) = delete;
};

#endif

/* EOF */
