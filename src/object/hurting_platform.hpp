//  SuperTux - Hurting Platform
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

#ifndef HEADER_SUPERTUX_OBJECT_HURTING_PLATFORM_HPP
#define HEADER_SUPERTUX_OBJECT_HURTING_PLATFORM_HPP

#include "object/platform.hpp"

/** Platform that hurts Tux and Badguys when touched */
class HurtingPlatform final : public Platform
{
public:
  HurtingPlatform(const ReaderMapping& reader);

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  static std::string class_name() { return "hurting_platform"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Hurting Platform"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Platform::get_class_types().add(typeid(HurtingPlatform)); }

private:
  HurtingPlatform(const HurtingPlatform&) = delete;
  HurtingPlatform& operator=(const HurtingPlatform&) = delete;
};

#endif

/* EOF */
