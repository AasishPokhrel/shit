//  Zeekling - flyer that swoops down when she spots the player
//  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_ZEEKLING_HPP
#define HEADER_SUPERTUX_BADGUY_ZEEKLING_HPP

#include "badguy/badguy.hpp"

class Zeekling final : public BadGuy
{
public:
  Zeekling(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual void active_update(float dt_sec) override;

  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;
  virtual bool is_freezable() const override;

  virtual std::string get_overlay_size() const override { return "2x1"; }
  static std::string class_name() { return "zeekling"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Zeekling"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Zeekling)); }
  virtual bool is_snipable() const override { return true; }

  virtual void on_flip(float height) override;

private:
  virtual bool collision_squished(MovingObject& object) override;

  bool should_we_dive();

  void set_speed(float speed);

  void fly();
  void charge();
  void dive();
  void recover();

  void on_bump_horizontal();
  void on_bump_vertical();

private:
  enum ZeeklingState {
    FLYING,
    CHARGING,
    DIVING,
    RECOVERING,
  };

private:
  float m_catch_pos;
  float m_target_y;
  Timer m_timer;
  ZeeklingState m_state;

private:
  Zeekling(const Zeekling&) = delete;
  Zeekling& operator=(const Zeekling&) = delete;
};

#endif

/* EOF */
