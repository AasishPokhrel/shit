//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_BADGUY_HPP
#define HEADER_SUPERTUX_BADGUY_BADGUY_HPP

#include "editor/object_option.hpp"
#include "object/moving_sprite.hpp"
#include "object/portable.hpp"
#include "supertux/physic.hpp"
#include "supertux/timer.hpp"

enum class Direction;
class Player;
class Bullet;

namespace
{
  static const std::string& DEFAULT_LIGHT_SPRITE = "images/objects/lightmap_light/lightmap_light-medium.sprite";
  static const std::string& DEFAULT_ICE_SPRITE = "images/creatures/overlays/iceoverlay/iceoverlay.sprite";
  static const std::string& DEFAULT_FIRE_SPRITE = "images/creatures/overlays/fireoverlay/fireoverlay.sprite";
}

/**
 * Base class for moving sprites that can hurt the Player.

 * @scripting
 * @summary A ""BadGuy"" that was given a name can be controlled by scripts.
 * @instances A ""BadGuy"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class BadGuy : public MovingSprite,
               public Portable
{
public:
  static void register_class(ssq::VM& vm);

public:
  BadGuy(const Vector& pos, const std::string& sprite_name, int layer = LAYER_OBJECTS,
         const std::string& light_sprite_name = DEFAULT_LIGHT_SPRITE,
         const std::string& ice_sprite_name = DEFAULT_ICE_SPRITE,
         const std::string& fire_sprite_name = DEFAULT_FIRE_SPRITE);
  BadGuy(const Vector& pos, Direction direction, const std::string& sprite_name, int layer = LAYER_OBJECTS,
         const std::string& light_sprite_name = DEFAULT_LIGHT_SPRITE,
         const std::string& ice_sprite_name = DEFAULT_ICE_SPRITE,
         const std::string& fire_sprite_name = DEFAULT_FIRE_SPRITE);
  BadGuy(const ReaderMapping& reader, const std::string& sprite_name, int layer = LAYER_OBJECTS,
         const std::string& light_sprite_name = DEFAULT_LIGHT_SPRITE,
         const std::string& ice_sprite_name = DEFAULT_ICE_SPRITE,
         const std::string& fire_sprite_name = DEFAULT_FIRE_SPRITE);
  BadGuy(const ReaderMapping& reader, const std::string& sprite_name, Direction default_direction, int layer = LAYER_OBJECTS,
         const std::string& light_sprite_name = DEFAULT_LIGHT_SPRITE,
         const std::string& ice_sprite_name = DEFAULT_ICE_SPRITE,
         const std::string & fire_sprite_name = DEFAULT_FIRE_SPRITE);

  /** Called when the badguy is drawn. The default implementation
      simply draws the badguy sprite on screen */
  virtual void draw(DrawingContext& context) override;

  /** Called each frame. The default implementation checks badguy
      state and calls active_update and inactive_update */
  virtual void update(float dt_sec) override;

  static std::string class_name() { return "badguy"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "BadGuy"; }
  static std::string display_name() { return _("Badguy"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(Portable)).add(typeid(BadGuy)); }

  virtual std::string get_overlay_size() const { return "1x1"; }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  /** Called when a collision with another object occurred. The
      default implementation calls collision_player, collision_solid,
      collision_badguy and collision_squished */
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  /** Called when a collision with tile with special attributes
      occurred */
  virtual void collision_tile(uint32_t tile_attributes) override;

  /** Set the badguy to kill/falling state, which makes him falling of
      the screen (his sprite is turned upside-down) */
  virtual void kill_fall();
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description Sets the badguy to kill/falling state, which makes it fall of the screen (its sprite is turned upside-down).
   */
  void kill();
#endif

  /** Call this, if you use custom kill_fall() or kill_squashed(GameObject& object) */
  virtual void run_dead_script();

  /** True if this badguy can break bricks or open bonusblocks in his
      current form. */
  virtual bool can_break() const { return false; }

  inline Vector get_start_position() const { return m_start_position; }
  inline void set_start_position(const Vector& vec) { m_start_position = vec; }

  virtual void grab(MovingObject& object, const Vector& pos, Direction dir) override;
  virtual void ungrab(MovingObject& object, Direction dir) override;
  virtual bool is_portable() const override;

  /**
   * @scripting
   * @description Kills the badguy by igniting it.
   */
  virtual void ignite();

  /** Called to revert a badguy when is_ignited() returns true */
  virtual void extinguish();

  /** Returns whether to call ignite() when a badguy gets hit by a fire bullet */
  virtual bool is_flammable() const;

  /** Returns whether this badguys is currently on fire */
  bool is_ignited() const;

  /** Called when hit by an ice bullet, and is_freezable() returns true. */
  virtual void freeze();

  /** Called to unfreeze the badguy. */
  virtual void unfreeze(bool melt = true);

  virtual bool is_freezable() const;

  /** Return true if this badguy can be hurt by tiles
      with the attribute "hurts" */
  virtual bool is_hurtable() const { return true; }

  /** Can enemy be sniped by sliding or swimboosting Tux?
    Returns false if enemy is spiky or too large */
  virtual bool is_snipable() const { return false; }

  virtual bool always_active() const { return false; }

  bool is_frozen() const;

  bool is_in_water() const;

  /** Get melting particle sprite filename */
  virtual std::string get_water_sprite() const {
    return "images/objects/water_drop/water_drop.sprite";
  }

  /** Returns true if the badguy can currently be affected by wind */
  virtual bool can_be_affected_by_wind() const;

  /** Adds velocity from wind */
  virtual void add_wind_velocity(const Vector& velocity, const Vector& end_speed);

  inline Physic& get_physic() { return m_physic; }

protected:
  enum State {
    STATE_INIT,
    STATE_INACTIVE,
    STATE_ACTIVE,
    STATE_SQUISHED,
    STATE_SQUISHED_FADING_OUT,
    STATE_FALLING,
    STATE_BURNING,
    STATE_MELTING,
    STATE_GROUND_MELTING,
    STATE_INSIDE_MELTING,
    STATE_GEAR
  };

protected:
  /** Called when the badguy collided with a player */
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit);

  /** Called when the badguy collided with solid ground */
  virtual void collision_solid(const CollisionHit& hit) override;

  virtual void on_flip(float height) override;

  /** Called when the badguy collided with another badguy */
  virtual HitResponse collision_badguy(BadGuy& other, const CollisionHit& hit);

  /** Called when the player hit the badguy from above. You should
      return true if the badguy was squished, false if squishing
      wasn't possible */
  virtual bool collision_squished(MovingObject& object);

  /** Called when the badguy collided with a bullet */
  virtual HitResponse collision_bullet(Bullet& bullet, const CollisionHit& hit);

  /** called each frame when the badguy is activated. */
  virtual void active_update(float dt_sec);

  /** called each frame when the badguy is not activated. */
  virtual void inactive_update(float dt_sec);

  /** called immediately before the first call to initialize */
  virtual void initialize();

  /** called when the badguy has been activated. (As a side effect the
      dir variable might have been changed so that it faces towards
      the player. */
  virtual void activate();

  /** called when the badguy has been deactivated */
  virtual void deactivate();

  /** Returns a vector of directions the badguy can be set to. */
  virtual std::vector<Direction> get_allowed_directions() const;

  void kill_squished(GameObject& object);

  void set_state(State state);
  inline State get_state() const { return m_state; }

  bool check_state_timer() {
    return m_state_timer.check();
  }

  /** returns a pointer to the nearest player or 0 if no player is available */
  Player* get_nearest_player() const;

  /** initial position of the enemy. Also the position where enemy
      respawns when after being deactivated. */
  bool is_offscreen() const;

  /** Returns true if we might soon fall at least @c height
      pixels. Minimum value for height is 1 pixel */
  bool might_fall(int height = 1);

  /** Update on_ground_flag judging by solid collision @c hit. This
      gets called from the base implementation of collision_solid, so
      call this when overriding collision_solid's default
      behaviour. */
  void update_on_ground_flag(const CollisionHit& hit);

  /** Returns true if we touched ground in the past frame This only
      works if update_on_ground_flag() gets called in
      collision_solid. */
  bool on_ground() const;

  /** Returns floor normal stored the last time when
      update_on_ground_flag was called and we touched something solid
      from above. */
  Vector get_floor_normal() const;

  /** Returns true if we were in STATE_ACTIVE at the beginning of the
      last call to update() */
  bool is_active() const;

  /** changes colgroup_active. Also calls set_group when badguy is in STATE_ACTIVE */
  void set_colgroup_active(CollisionGroup group);

private:
  void try_activate();

protected:
  static const int s_normal_max_drop_height = 600;

protected:
  Physic m_physic;

public:
  /** Count this badguy to the statistics? This value should not be
      changed during runtime. */
  bool m_countMe;

protected:
  /** true if initialize() has already been called */
  bool m_is_initialized;

  Vector m_start_position;

  /** The direction we currently face in */
  Direction m_dir;

  /** The direction we initially faced in */
  Direction m_start_dir;

  bool m_frozen;
  bool m_ignited; /**< true if this badguy is currently on fire */
  bool m_in_water; /** < true if the badguy is currently in water */

  std::string m_dead_script; /**< script to execute when badguy is killed */

  float m_melting_time;

  SpritePtr m_lightsprite;
  SpritePtr m_freezesprite;
  SpritePtr m_firesprite;
  bool m_glowing;
  bool m_water_affected;

  Timer m_unfreeze_timer;

  /** floor normal stored the last time when update_on_ground_flag was
      called and we touched something solid from above */
  Vector m_floor_normal;

  /** Used for the might_fall function.
      Represents the tile data of the detected slope. */
  int m_detected_slope;

private:
  State m_state;

  /** true if state was STATE_ACTIVE at the beginning of the last call
      to update() */
  bool m_is_active_flag;

  Timer m_state_timer;

  /** true if we touched something solid from above and
      update_on_ground_flag was called last frame */
  bool m_on_ground_flag;


  /** CollisionGroup the badguy should be in while active */
  CollisionGroup m_colgroup_active;

  /** The alpha value at the time the Badguy begins to fadeout */
  float m_alpha_before_fadeout;

  Color m_flame_color;
  Timer m_flame_timer;

private:
  BadGuy(const BadGuy&) = delete;
  BadGuy& operator=(const BadGuy&) = delete;
};

#endif

/* EOF */
