//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_BADGUY_MRICEBLOCK_HPP
#define HEADER_SUPERTUX_BADGUY_MRICEBLOCK_HPP

#include "badguy/walking_badguy.hpp"

class MrIceBlock : public WalkingBadguy
{
public:
  MrIceBlock(const ReaderMapping& reader,
             const std::string& sprite_name = "images/creatures/iceblock/iceblock.sprite");

  virtual void initialize() override;
  virtual HitResponse collision(MovingObject& object, const CollisionHit& hit) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;

  virtual void active_update(float dt_sec) override;

  virtual void grab(MovingObject& object, const Vector& pos, Direction dir) override;
  virtual void ungrab(MovingObject& object, Direction dir) override;
  virtual bool is_portable() const override;

  virtual bool can_break() const override;

  virtual void ignite() override;
  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;

  static std::string class_name() { return "mriceblock"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Mr. Iceblock"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(MrIceBlock)); }

  virtual bool is_snipable() const override { return ice_state != ICESTATE_KICKED; }
  virtual bool is_freezable() const override;

  virtual GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

  bool can_break();

protected:
  enum IceState {
    ICESTATE_NORMAL,
    ICESTATE_FLAT,
    ICESTATE_GRABBED,
    ICESTATE_KICKED,
    ICESTATE_WAKING
  };

protected:
  virtual bool collision_squished(MovingObject& object) override;
  void set_state(IceState state);

private:
  enum Type {
    NORMAL,
    LAPTOP
  };

private:
  IceState ice_state;
  Timer nokick_timer;
  Timer flat_timer;
  int squishcount;

private:
  MrIceBlock(const MrIceBlock&) = delete;
  MrIceBlock& operator=(const MrIceBlock&) = delete;
};

#endif

/* EOF */
