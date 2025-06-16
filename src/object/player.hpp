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

#ifndef HEADER_SUPERTUX_OBJECT_PLAYER_HPP
#define HEADER_SUPERTUX_OBJECT_PLAYER_HPP

#include "sprite/sprite_ptr.hpp"
#include "supertux/direction.hpp"
#include "supertux/moving_object.hpp"
#include "supertux/object_remove_listener.hpp"
#include "supertux/physic.hpp"
#include "supertux/player_status.hpp"
#include "supertux/sequence.hpp"
#include "supertux/timer.hpp"
#include "video/layer.hpp"
#include "video/surface_ptr.hpp"

#include <array>
#include <list>

class BadGuy;
class Climbable;
class Controller;
class CodeController;
class Key;
class Portable;

extern const float TUX_INVINCIBLE_TIME_WARNING;

/**
 * @scripting
 * @summary This module contains methods controlling the player. (No, SuperTux doesn't use mind control. ""Player"" refers to the type of the player object.)
 * @instances The first player can be accessed using ""Tux"", or ""sector.Tux"" from the console.
              All following players (2nd, 3rd, etc...) can be accessed by ""Tux{index}"".
              For example, to access the 2nd player, use ""Tux1"" (or ""sector.Tux1"" from the console).
 */
class Player final : public MovingObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  enum FallMode { ON_GROUND, JUMPING, TRAMPOLINE_JUMP, FALLING };

private:
  class GrabListener final : public ObjectRemoveListener
  {
  public:
    GrabListener(Player& player) : m_player(player)
    {}

    virtual void object_removed(GameObject* object) override {
      m_player.ungrab_object(object);
    }

  private:
    Player& m_player;

  private:
    GrabListener(const GrabListener&) = delete;
    GrabListener& operator=(const GrabListener&) = delete;
  };

public:
  static Color get_player_color(int id);

public:
  Player(PlayerStatus& player_status, const std::string& name, int player_id);
  ~Player() override;

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void collision_tile(uint32_t tile_attributes) override;
  virtual void on_flip(float height) override;
  virtual bool is_saveable() const override { return false; }
  virtual bool is_singleton() const override { return false; }
  virtual bool has_object_manager_priority() const override { return true; }
  virtual std::string get_exposed_class_name() const override { return "Player"; }
  virtual void remove_me() override;
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(Player)); }

  inline int get_id() const { return m_id; }
  void set_id(int id);

  virtual int get_layer() const override { return LAYER_OBJECTS + 1; }

  inline void set_controller(const Controller* controller) { m_controller = controller; }
  /** Level solved. Don't kill Tux any more. */
  void set_winning();
  inline bool is_winning() const { return m_winning; }

  // Tux can only go this fast. If set to 0 no special limit is used, only the default limits.
  inline void set_speedlimit(float limit) { m_speedlimit = limit; }
  inline float get_speedlimit() const { return m_speedlimit; }

  inline const Controller& get_controller() const { return *m_controller; }

  /**
   * @scripting
   * @description Uses a scriptable controller for all user input (or restores controls).
   * @param bool $enable
   */
  void use_scripting_controller(bool enable);
  /**
   * @scripting
   * @description Instructs the scriptable controller to press or release a button.
   * @param string $control Can be “left”, “right”, “up”, “down”, “jump”, “action”, “start”, “escape”,
      “menu-select”, “menu-select-space”, “menu-back”, “remove”, “cheat-menu”, “debug-menu”, “console”,
      “peek-left”, “peek-right”, “peek-up” or “peek-down”.
   * @param bool $pressed
   */
  void do_scripting_controller(const std::string& control, bool pressed);

  /** Move the player to a different sector, including any objects that it points to, or references. */
  void move_to_sector(Sector& other);

  /**
   * Make Tux invincible for a short amount of time.
   */
  void make_invincible();
  void make_temporarily_safe(float safe_time);

  inline bool is_invincible() const { return m_invincible_timer.started(); }
  inline bool is_dying() const { return m_dying; }

  /**
   * Returns true if the player is currently alive
   * (not dying or dead)
   */
  inline bool is_alive() const { return !is_dying() && !is_dead(); }

  /**
   * Returns true if the player can be controlled.
   * (alive and not currently in a win sequence)
   */
  inline bool is_active() const { return is_alive() && !is_winning(); }

  inline Direction peeking_direction_x() const { return m_peekingX; }
  inline Direction peeking_direction_y() const { return m_peekingY; }

  /**
   * @scripting
   * @description Hurts Tux.
   * @param bool $completely If true, he will be killed even if he had "grow" or a superior bonus.
   */
  void kill(bool completely);

  void set_pos(const Vector& vector) override;

  /**
   * @scripting
   * @description Gives Tux the specified bonus unless Tux’s current bonus is superior.
   * @param string $bonus Can be "grow", "fireflower", "iceflower", "airflower" or "earthflower" at the moment.
   */
  bool add_bonus(const std::string& bonus);
  /**
   * @scripting
   * @description Gives Tux the specified bonus.
   * @param string $bonus Can be "grow", "fireflower", "iceflower", "airflower" or "earthflower" at the moment.
   */
  bool set_bonus(const std::string& bonus);
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description Returns Tux's current bonus.
   */
  std::string get_bonus() const;
#endif

  /**
   * @scripting
   * @description Gives the player a number of coins.${SRG_TABLENEWPARAGRAPH}
                  If count is a negative amount of coins, that number of coins will be taken
                  from the player (until the number of coins the player has is 0, when it
                  will stop changing).
   * @param int $count
   */
  inline void add_coins(int count) { m_player_status.add_coins(count); }
  /**
   * @scripting
   * @description Returns the number of coins the player currently has.
   */
  inline int get_coins() const { return m_player_status.coins; }

  /** picks up a bonus, taking care not to pick up lesser bonus items than we already have

      @returns true if the bonus has been set (or was already good enough)
               false if the bonus could not be set (for example no space for big tux) */
  bool add_bonus(BonusType type, bool animate = false);

  /** like add_bonus, but can also downgrade the bonus items carried */
  bool set_bonus(BonusType type, bool animate = false);
  inline BonusType get_bonus() const { return m_player_status.bonus[m_id]; }

  std::string bonus_to_string() const;

  inline PlayerStatus& get_status() const { return m_player_status; }

  /**
   * @scripting
   * @description Start kick animation.
   */
  void kick();

  /**
   * @scripting
   * @description Gets the player's current action/animation.
   */
  std::string get_action() const;

  /**
   * @scripting
   * @description Play cheer animation.${SRG_TABLENEWPARAGRAPH}
   *              This might need some space and behave in an unpredictable way. It's best to use this at level end.
   */
  void do_cheer();

  /**
   * @scripting
   * @description Makes Tux duck down, if possible. Won't last long, as long as input is enabled.
   */
  void do_duck();

  /**
   * @scripting
   * @description Makes Tux stand back up, if possible.
   */
  void do_standup();
  /**
   * @scripting
   * @description Makes Tux stand back up, if possible.
   */
  void do_standup(bool force_standup);

  /**
   * @scripting
   * @description Makes Tux do a backflip, if possible.
   */
  void do_backflip();

  /**
   * @scripting
   * @description Makes Tux jump in the air, if possible.
   * @param float $yspeed Sensible values are negative - unless we want to jump into the ground of course.
   */
  void do_jump(float yspeed);

  /** Adds velocity to the player (be careful when using this) */
  void add_velocity(const Vector& velocity);

  /** Adds velocity to the player until given end speed is reached */
  void add_velocity(const Vector& velocity, const Vector& end_speed);

  /** Returns the current velocity of the player */
  inline Vector get_velocity() const { return m_physic.get_velocity(); }
  /**
   * @scripting
   * @description Returns Tux’s velocity in X direction.
   */
  inline float get_velocity_x() const { return m_physic.get_velocity_x(); }
  /**
   * @scripting
   * @description Returns Tux’s velocity in Y direction.
   */
  inline float get_velocity_y() const { return m_physic.get_velocity_y(); }
  /**
   * @scripting
   * @description Sets the velocity of the player to a programmable/variable speed.
   * @param float $x The speed Tux will move on the x axis.
   * @param float $y The speed Tux will move on the y axis.
   */
  inline void set_velocity(float x, float y) { m_physic.set_velocity(x, y); }

  void bounce(BadGuy& badguy);
  inline void override_velocity() { m_velocity_override = true; }

  inline bool is_dead() const { return m_dead; }
  inline bool is_big() const { return get_bonus() != BONUS_NONE; }
  inline bool is_stone() const { return m_stone; }
  inline bool is_sliding() const { return m_sliding; }
  inline bool is_swimming() const { return m_swimming; }
  inline bool is_swimboosting() const { return m_swimboosting; }
  inline bool is_water_jumping() const { return m_water_jump; }
  inline bool is_skidding() const { return m_skidding_timer.started(); }
  inline bool is_scripting_activated() const { return !m_deactivated; }
  inline float get_swimming_angle() const { return m_swimming_angle; }

  /**
   * @scripting
   * @description Set Tux visible or invisible.
   * @param bool $visible
   */
  inline void set_visible(bool visible) { m_visible = visible; }
  /**
   * @scripting
   * @description Returns ""true"" if Tux is currently visible (has not been set invisible by the ""set_visible()"" method).
   */
  inline bool get_visible() const { return m_visible; }

  /**
   * @scripting
   * @description Make tux invincible without the star effect.
   * @param bool $safe
   */
  void set_is_intentionally_safe(bool safe);
  /**
   * @scripting
   * @description Returns ""true"" if Tux is currently intentionally safe.
   */
  bool get_is_intentionally_safe() const;

  inline bool on_ground() const { return m_on_ground_flag; }
  inline void set_on_ground(bool flag) { m_on_ground_flag = flag; }

  inline Portable* get_grabbed_object() const { return m_grabbed_object; }
  inline void stop_grabbing() { ungrab_object(); }

  /**
   * @scripting
   * @description Returns whether the player is carrying a certain object.
   * @param string $name Name of the portable object to check for.
   */
  bool has_grabbed(const std::string& name) const;

  /**
   * @scripting
   * @description Switches ghost mode on/off. Lets Tux float around and through solid objects.
   * @param bool $enable
   */
  void set_ghost_mode(bool enable);
  /**
   * @scripting
   * @description Returns whether ghost mode is currently enabled.
   */
  inline bool get_ghost_mode() const { return m_ghost_mode; }

  /** Changes height of bounding box.
      Returns true if successful, false otherwise */
  bool adjust_height(float new_height, float bottom_offset = 0);

  /**
   * @scripting
   * @description Orders the current ""GameSession"" to start a sequence.
   * @param string $sequence_name One of “stoptux”, “endsequence” or “fireworks”.
   */
  void trigger_sequence(const std::string& sequence_name);

  /** Orders the current GameSession to start a sequence
      @param sequence_name Name of the sequence to start
      @param data Custom additional sequence data */
  void trigger_sequence(const std::string& sequence_name, const SequenceData* data);

  /** Orders the current GameSession to start a sequence
      @param sequence Sequence to start
      @param data Custom additional sequence data */
  void trigger_sequence(Sequence seq, const SequenceData* data = nullptr);

  /** Requests that the player start climbing the given Climbable */
  void start_climbing(Climbable& climbable);

  /** Requests that the player stop climbing the given Climbable */
  void stop_climbing(Climbable& climbable);

  inline Physic& get_physic() { return m_physic; }

  /**
   * @scripting
   * @description Give control back to user/scripting.
   */
  void activate();
  /**
   * @scripting
   * @description Deactivate user/scripting input for Tux.
     Carried items like trampolines won't be dropped.
   */
  void deactivate();

  /**
   * @scripting
   * @description Gets whether the current input on the keyboard/controller/touchpad has been pressed.
   * @param string $input Can be “left”, “right”, “up”, “down”, “jump”, “action”, "item", “start”, “escape”,
      “menu-select”, “menu-select-space”, “menu-back”, “remove”, “cheat-menu”, “debug-menu”, “console”,
      “peek-left”, “peek-right”, “peek-up” or “peek-down”.
   */
  bool get_input_pressed(const std::string& input);
  /**
   * @scripting
   * @description Gets whether the current input on the keyboard/controller/touchpad is being held.
   * @param string $input Valid values are listed above.
   */
  bool get_input_held(const std::string& input);
  /**
   * @scripting
   * @description Gets whether the current input on the keyboard/controller/touchpad has been released.
   * @param string $input Valid values are listed above.
   */
  bool get_input_released(const std::string& input);

  /**
   * @scripting
   * @description Makes Tux walk.
   * @param float $speed
   */
  inline void walk(float speed) { m_physic.set_velocity_x(speed); }
  /**
   * @scripting
   * @description Face Tux in the proper direction.
   * @param bool $right Set to ""true"" to make Tux face right, ""false"" to face left.
   */
  void set_dir(bool right);
  void stop_backflipping();

  /**
   * @scripting
   * @description Ejects the item in the player's Item Pocket.
   */
  void eject_item_pocket();

  /**
   * @scripting
   * @description Returns the item currently in the player's Item Pocket as a ""BONUS"" enum value.
   */
  int get_item_pocket() const;

  /**
   * @scripting
   * @description Ejects the item in the player's Item Pocket.
   */
  void set_item_pocket(int bonus);

  void position_grabbed_object(bool teleport = false);
  bool try_grab();

  /** Boosts Tux in a certain direction, sideways. Useful for bumpers/walljumping. */
  void sideways_push(float delta);

  void multiplayer_prepare_spawn();

  inline void set_ending_direction(int direction) { m_ending_direction = direction; }
  inline int get_ending_direction() const { return m_ending_direction; }

  inline const std::vector<Key*>& get_collected_keys() const { return m_collected_keys; }
  void add_collected_key(Key* key);
  void remove_collected_key(Key* key);

  bool track_state() const override { return false; }

private:
  void handle_input();
  void handle_input_ghost(); /**< input handling while in ghost mode */
  void handle_input_climbing(); /**< input handling while climbing */
  void handle_input_rolling();

  void handle_input_swimming();

  void handle_horizontal_input();
  void handle_vertical_input();

  /** Set Tux's position, reset state and velocity. */
  void set_pos_reset(const Vector& vector);

  void do_jump_apex();
  void early_jump_apex();

  void slide();
  void swim(float pointx, float pointy, bool boost);

  BonusType string_to_bonus(const std::string& bonus) const;

  /** slows Tux down a little, based on where he's standing */
  void apply_friction();

  void check_bounds();

  /**
   * Ungrabs the currently grabbed object, if any. Only call with its argument
   * from an ObjectRemoveListener.
   */
  void ungrab_object(GameObject* gameobject = nullptr);

  void next_target();
  void prev_target();

  void multiplayer_respawn();

  void stop_rolling(bool violent = true);

private:
  int m_id;
  std::unique_ptr<UID> m_target; /**< (Multiplayer) If not null, then the player does not exist in game and is offering the player to spawn at that player's position */
  bool m_deactivated;

  const Controller* m_controller;
  std::unique_ptr<CodeController> m_scripting_controller; /**< This controller is used when the Player is controlled via scripting */
  PlayerStatus& m_player_status;
  bool m_duck;
  bool m_crawl;
  bool m_dead;
  bool m_dying;
  bool m_winning;
  bool m_backflipping;
  int  m_backflip_direction;
  Direction m_peekingX;
  Direction m_peekingY;
  bool m_stone;
  bool m_sliding;
  bool m_slidejumping;
  bool m_swimming;
  bool m_swimboosting;
  bool m_no_water;
  bool m_on_left_wall;
  bool m_on_right_wall;
  bool m_in_walljump_tile;
  bool m_can_walljump;
  float m_boost;
  float m_speedlimit;
  bool m_velocity_override;
  const Controller* m_scripting_controller_old; /**< Saves the old controller while the scripting_controller is used */
  bool m_jump_early_apex;
  bool m_on_ice;
  bool m_ice_this_frame;
  //SpritePtr m_santahatsprite;
  SpritePtr m_multiplayer_arrow;

  // Multiplayer tag stuff (number displayed over the players)
  Timer m_tag_timer;
  std::unique_ptr<FadeHelper> m_tag_fade;
  float m_tag_alpha;
  bool m_has_moved; // If the player sent input to move the player

public:
  Direction m_dir;

private:
  Direction m_old_dir;

public:
  float m_last_ground_y;
  FallMode m_fall_mode;

private:
  bool m_on_ground_flag;
  bool m_jumping;
  bool m_can_jump;
  Timer m_jump_button_timer; /**< started when player presses the jump button; runs until Tux jumps or JUMP_GRACE_TIME runs out */
  Timer m_coyote_timer; /**< started when Tux falls off a ledge; runs until Tux jumps or COYOTE_TIME runs out */
  bool m_wants_buttjump;
  bool m_buttjump_stomp;

public:
  bool m_does_buttjump;
  Timer m_invincible_timer;

private:
  Timer m_skidding_timer;
  Timer m_post_damage_safety_timer;
  Timer m_temp_safety_timer;

  /**
   * @scripting
   * @description Determines whether Tux is invincible.
   */
  bool m_is_intentionally_safe;

  Timer m_kick_timer;
  Timer m_buttjump_timer;

public:
  Timer m_dying_timer;

private:
  Timer m_second_growup_sound_timer;
  bool m_growing;
  Timer m_backflip_timer;

  Physic m_physic;

  /**
   * @scripting
   * @description Determines whether Tux is visible.
   */
  bool m_visible;

  Portable* m_grabbed_object;
  std::unique_ptr<ObjectRemoveListener> m_grabbed_object_remove_listener;
  bool m_released_object;

  SpritePtr m_sprite; /**< The main sprite representing Tux */

  float m_swimming_angle;
  float m_swimming_accel_modifier;
  bool m_water_jump;

  SurfacePtr m_airarrow; /**< arrow indicating Tux' position when he's above the camera */

  SpritePtr m_bubbles_sprite; /**< bubble particles sprite for swimming */
  Timer m_bubble_timer; /**< timer for spawning bubble particles */
  std::list<std::pair<SpritePtr, Vector>> m_active_bubbles; /**< active bubble particles */

  Vector m_floor_normal;

  bool m_ghost_mode; /**< indicates if Tux should float around and through solid objects */

  Timer m_unduck_hurt_timer; /**< if Tux wants to stand up again after ducking and cannot, this timer is started */

  Timer m_idle_timer;
  unsigned int m_idle_stage;

  Climbable* m_climbing; /**< Climbable object we are currently climbing, null if none */

  int m_ending_direction;
  std::vector<Key*> m_collected_keys;

  float m_last_sliding_angle;
  float m_current_sliding_angle;
  float m_target_sliding_angle;
  Timer m_sliding_rotation_timer;
  bool m_is_slidejump_falling;
  bool m_was_crawling_before_slide;

private:
  Player(const Player&) = delete;
  Player& operator=(const Player&) = delete;
};

#endif

/* EOF */
