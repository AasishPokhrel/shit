//  SuperTux - Mole Badguy
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

#ifndef HEADER_SUPERTUX_BADGUY_MOLE_HPP
#define HEADER_SUPERTUX_BADGUY_MOLE_HPP

#include "badguy/badguy.hpp"

class Mole final : public BadGuy
{
public:
  Mole(const ReaderMapping& );

  virtual void kill_fall() override;
  virtual HitResponse collision_badguy(BadGuy& , const CollisionHit& ) override;
  virtual bool collision_squished(MovingObject& object) override;

  virtual void activate() override;
  virtual void active_update(float) override;

  virtual void ignite() override;

  static std::string class_name() { return "mole"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Mole"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Mole)); }

  virtual bool is_snipable() const override { return true; }

  virtual void on_flip(float height) override;

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  enum MoleState {
    PRE_THROWING,
    THROWING,
    POST_THROWING,
    PEEKING,
    DEAD,
    BURNING
  };

private:
  void set_state(MoleState new_state);
  void throw_rock();

private:
  MoleState state;
  Timer timer;
  Timer throw_timer;
  int cycle_num;

private:
  Mole(const Mole&) = delete;
  Mole& operator=(const Mole&) = delete;
};

#endif

/* EOF */
