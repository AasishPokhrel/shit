//  SuperTux
//  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_SNOWMAN_HPP
#define HEADER_SUPERTUX_BADGUY_SNOWMAN_HPP

#include "badguy/walking_badguy.hpp"

class Snowman final : public WalkingBadguy
{
public:
  Snowman(const ReaderMapping& reader);

  static std::string class_name() { return "snowman"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Snowman"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(Snowman)); }

protected:
  void loose_head();
  virtual HitResponse collision_bullet(Bullet& bullet, const CollisionHit& hit) override;
  virtual bool collision_squished(MovingObject& object) override;

private:
  Snowman(const Snowman&) = delete;
  Snowman& operator=(const Snowman&) = delete;
};

#endif

/* EOF */
