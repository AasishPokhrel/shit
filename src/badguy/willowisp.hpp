//  SuperTux - "Will-O-Wisp" Badguy
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

#ifndef HEADER_SUPERTUX_BADGUY_WILLOWISP_HPP
#define HEADER_SUPERTUX_BADGUY_WILLOWISP_HPP

#include "badguy/badguy.hpp"
#include "object/path_object.hpp"

class SoundSource;

/**
 * @scripting
 * @summary A ""WillOWisp"" that was given a name can be controlled by scripts.
 * @instances A ""WillOWisp"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class WillOWisp final : public BadGuy,
                        public PathObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  WillOWisp(const ReaderMapping& reader);

  virtual void finish_construction() override;
  virtual void after_editor_set() override;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void active_update(float dt_sec) override;
  virtual bool is_flammable() const override { return false; }
  virtual bool is_freezable() const override { return false; }
  virtual bool is_hurtable() const override { return false; }
  virtual void kill_fall() override { vanish(); }

  void goto_node(int node_idx);

  /**
   * @scripting
   * @description Sets the state of the WillOWisp.
   * @param string $state One of the following: "stopped", "move_path" (moves along a path),
      "move_path_track" (moves along a path but catches Tux when he is near), "normal" (starts tracking Tux when he is near enough),
      "vanish".
   */
  void set_state(const std::string& state);

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

  static std::string class_name() { return "willowisp"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "WillOWisp"; }
  static std::string display_name() { return _("Will o' Wisp"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(PathObject)).add(typeid(WillOWisp)); }
  virtual void editor_update() override;

  virtual ObjectSettings get_settings() override;
  virtual void move_to(const Vector& pos) override;

  virtual void on_flip(float height) override;

  /** make WillOWisp vanish */
  void vanish();

  inline Color get_color() const { return m_color; }

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  virtual bool collides(MovingObject& other, const CollisionHit& hit) const override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  void synchronize_position_from_path();

private:
  enum MyState {
    STATE_STOPPED, STATE_IDLE, STATE_TRACKING, STATE_VANISHING, STATE_WARPING,
    STATE_PATHMOVING, STATE_PATHMOVING_TRACK
  };

private:
  MyState m_mystate;

  std::string m_target_sector;
  std::string m_target_spawnpoint;
  std::string m_hit_script;

  std::unique_ptr<SoundSource> m_sound_source;
  float m_flyspeed;
  float m_track_range;
  float m_vanish_range;

  Color m_color;

  int m_starting_node;

private:
  WillOWisp(const WillOWisp&) = delete;
  WillOWisp& operator=(const WillOWisp&) = delete;
};

#endif

/* EOF */
