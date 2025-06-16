//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_WALKING_CANDLE_HPP
#define HEADER_SUPERTUX_BADGUY_WALKING_CANDLE_HPP

#include "badguy/walking_badguy.hpp"

class WalkingCandle final : public WalkingBadguy
{
public:
  WalkingCandle(const ReaderMapping& reader);

  virtual bool is_freezable() const override;
  virtual bool is_flammable() const override;

  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  virtual void kill_fall() override;

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;
  static std::string class_name() { return "walking_candle"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Walking Candle"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(WalkingCandle)); }

private:
  Color m_lightcolor;

private:
  WalkingCandle(const WalkingCandle&) = delete;
  WalkingCandle& operator=(const WalkingCandle&) = delete;
};

#endif

/* EOF */
