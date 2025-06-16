//  SuperTux - Boss "Yeti"
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

#ifndef HEADER_SUPERTUX_BADGUY_YETI_HPP
#define HEADER_SUPERTUX_BADGUY_YETI_HPP

#include "badguy/boss.hpp"

class Yeti final : public Boss
{
public:
  Yeti(const ReaderMapping& mapping);

  virtual void draw(DrawingContext& context) override;
  virtual void initialize() override;
  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual bool collision_squished(MovingObject& object) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual void kill_fall() override;

  static std::string class_name() { return "yeti"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Yeti"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Boss::get_class_types().add(typeid(Yeti)); }

  void kill_squished(GameObject& object);

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  enum YetiState {
    ANNOUNCE,
    RUN,
    FALL,
    JUMP,
    IDLE,
    THROW,
    THROW_BIG,
    STOMP,
    DIZZY,
    BUSTED,
    THROW_TUX /// Yeti grabs Tux and throws him away. Take that!
  };

private:
  void announce(bool taunt);
  void run(bool change_state);
  void jump(float velocity);
  void idle(bool stomp, float waitduration = 0);
  void throw_snowball();
  void throw_big_snowball();
  void stomp();
  void throw_tux();
  void turn_dizzy();
  void bust();

  bool is_idle();

  void drop_stalactite();
  void summon_snowball();
  void summon_big_snowball();

  void take_hit(Player& player);

  void add_snow_explosions();
  void recalculate_pos();

private:
  YetiState m_state;
  YetiState m_next_state;
  Timer m_state_timer;
  Timer m_safe_timer;

  bool m_attacked;
  int m_attack_count;

  float m_left_stand_x;
  float m_right_stand_x;
  float m_left_jump_x;
  float m_right_jump_x;

  bool m_just_hit;
  bool m_pinch_announced;
  bool m_grabbed_tux;

  class SnowExplosionParticle: public BadGuy
  {
  public:
    SnowExplosionParticle(const Vector& pos, const Vector& velocity);
    virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Yeti::SnowExplosionParticle)); }
  };

private:
  Yeti(const Yeti&) = delete;
  Yeti& operator=(const Yeti&) = delete;
};

#endif

/* EOF */
