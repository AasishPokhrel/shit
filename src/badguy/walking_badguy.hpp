//  SuperTux - WalkingBadguy
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

#ifndef HEADER_SUPERTUX_BADGUY_WALKING_BADGUY_HPP
#define HEADER_SUPERTUX_BADGUY_WALKING_BADGUY_HPP

#include "badguy/badguy.hpp"

class Timer;

/** Base class for Badguys that walk on the floor. */
class WalkingBadguy : public BadGuy
{
public:
  enum class LedgeBehavior
  {
    STRICT, /**< Do not fall off any ledge at all. */
    SMART, /**< Do not fall off any ledge, but still go down slopes. */
    NORMAL, /**< Fall off any ledge, unless the ledge is too tall (600px) or the ledge falls offscreen. */
    FALL /**< Fall off any ledge. */
  };

public:
  WalkingBadguy(const Vector& pos,
                const std::string& sprite_name,
                const std::string& walk_left_action,
                const std::string& walk_right_action,
                int layer = LAYER_OBJECTS,
                const std::string& light_sprite_name = "images/objects/lightmap_light/lightmap_light-medium.sprite");
  WalkingBadguy(const Vector& pos, Direction direction,
                const std::string& sprite_name,
                const std::string& walk_left_action,
                const std::string& walk_right_action,
                int layer = LAYER_OBJECTS,
                const std::string& light_sprite_name = "images/objects/lightmap_light/lightmap_light-medium.sprite");
  WalkingBadguy(const ReaderMapping& reader,
                const std::string& sprite_name,
                const std::string& walk_left_action,
                const std::string& walk_right_action,
                int layer = LAYER_OBJECTS,
                const std::string& light_sprite_name = "images/objects/lightmap_light/lightmap_light-medium.sprite");
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(WalkingBadguy)); }

  virtual void initialize() override;
  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual void freeze() override;
  virtual void unfreeze(bool melt = true) override;

  void active_update(float dt_sec, float target_velocity, float modifier = 1.f);
  /** used by objects that should make badguys not turn around when they are walking on them */
  void override_stay_on_platform() { m_stay_on_platform_overridden = true; }

  inline float get_velocity_x() const { return m_physic.get_velocity_x(); }
  inline float get_velocity_y() const { return m_physic.get_velocity_y(); }
  inline void set_velocity_x(float vx) { m_physic.set_velocity_x(vx); }
  inline void set_velocity_y(float vy) { m_physic.set_velocity_y(vy); }

  /** Adds velocity to the badguy (be careful when using this) */
  void add_velocity(const Vector& velocity);

  inline float get_walk_speed() const { return walk_speed; }
  void set_walk_speed (float);
  inline bool is_active() const { return BadGuy::is_active(); }

  /** Set max_drop_height depending on the given behavior */
  void set_ledge_behavior(LedgeBehavior behavior);

protected:
  void turn_around();

protected:
  std::string walk_left_action;
  std::string walk_right_action;
  float walk_speed;
  int max_drop_height; /**< Maximum height of drop before we will turn around, or -1 to just drop from any ledge */
  Timer turn_around_timer;
  int turn_around_counter; /**< counts number of turns since turn_around_timer was started */
  bool m_stay_on_platform_overridden;

private:
  WalkingBadguy(const WalkingBadguy&) = delete;
  WalkingBadguy& operator=(const WalkingBadguy&) = delete;
};

#endif

/* EOF */
