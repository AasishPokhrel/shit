//  SuperTux - Badguy "Snail"
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

#ifndef HEADER_SUPERTUX_BADGUY_SNAIL_HPP
#define HEADER_SUPERTUX_BADGUY_SNAIL_HPP

#include "badguy/walking_badguy.hpp"

/** Badguy "Snail" - a snail-like creature that can be flipped and
    tossed around at an angle */
class Snail final :
  public WalkingBadguy
{
public:
  Snail(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual bool can_break() const override;

  virtual void active_update(float dt_sec) override;

  virtual bool is_freezable() const override;
  virtual bool is_snipable() const override;

  static std::string class_name() { return "snail"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Snail"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(Snail)); }

  virtual GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

  virtual bool is_portable() const override;
  virtual void ungrab(MovingObject& , Direction dir_) override;
  virtual void grab(MovingObject&, const Vector& pos, Direction dir_) override;

protected:
  virtual bool collision_squished(MovingObject& object) override;

private:
  void be_normal(); /**< switch to state STATE_NORMAL */
  void be_guard(); /**< switch to state STATE_GUARD_SHAKE (prepare for STATE_GUARD) */
  void be_flat(); /**< switch to state STATE_FLAT */
  void be_kicked(bool upwards); /**< switch to state STATE_KICKED_DELAY */
  void be_grabbed();
  void wake_up();

private:
  enum State {
    STATE_NORMAL, /**< walking around */
    STATE_GUARD_SHAKE, /**< short delay before switching to STATE_GUARD, plays respective animation */
    STATE_GUARD, /**< short guarding state, periodically activated on corrupted snails while walking */
    STATE_GUARD_RETRACT, /**< short delay before switching back to STATE_NORMAL, plays respective animation */
    STATE_FLAT, /**< flipped upside-down */
    STATE_WAKING, /**< is waking up */
    STATE_KICKED_DELAY, /**< short delay before being launched */
    STATE_KICKED, /**< launched */
    STATE_GRABBED, /**< grabbed by tux */
  };
  enum Type {
    NORMAL,
    CORRUPTED
  };

private:
  State state;
  Timer kicked_delay_timer; /**< wait time until switching from STATE_KICKED_DELAY to STATE_KICKED */
  Timer flat_timer;
  Timer m_guard_timer;
  Timer m_guard_end_timer;
  int squishcount;

private:
  Snail(const Snail&) = delete;
  Snail& operator=(const Snail&) = delete;
};

#endif

/* EOF */
