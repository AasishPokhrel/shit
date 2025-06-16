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

#ifndef HEADER_SUPERTUX_SUPERTUX_GAME_SESSION_HPP
#define HEADER_SUPERTUX_SUPERTUX_GAME_SESSION_HPP

#include "supertux/screen.hpp"
#include "util/currenton.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <queue>

#include <simplesquirrel/table.hpp>

#include "math/vector.hpp"
#include "squirrel/squirrel_scheduler.hpp"
#include "squirrel/squirrel_util.hpp"
#include "supertux/game_object.hpp"
#include "supertux/player_status.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/sequence.hpp"
#include "supertux/timer.hpp"
#include "video/surface_ptr.hpp"

class CodeController;
class DrawingContext;
class EndSequence;
class Level;
class Player;
class Sector;
class Statistics;
class Savegame;

/** Screen that runs a Level, where Players run and jump through Sectors. */
class GameSession final : public Screen,
                          public Currenton<GameSession>
{
private:
  struct SpawnPoint
  {
    /* If a spawnpoint is set, the spawn position shall not, and vice versa. */
    SpawnPoint(const std::string& sector_,
               const Vector& position_,
               bool is_checkpoint_ = false) :
      sector(sector_),
      spawnpoint(),
      position(position_),
      is_checkpoint(is_checkpoint_)
    {}
    SpawnPoint(const std::string& sector_,
               const std::string& spawnpoint_,
               bool is_checkpoint_ = false) :
      sector(sector_),
      spawnpoint(spawnpoint_),
      position(),
      is_checkpoint(is_checkpoint_)
    {}

    std::string sector;
    std::string spawnpoint;
    Vector position;
    bool is_checkpoint;
  };

public:
  GameSession(const std::string& levelfile, Savegame& savegame, Statistics* statistics = nullptr);

  virtual void draw(Compositor& compositor) override;
  virtual void update(float dt_sec, const Controller& controller) override;
  virtual void setup() override;
  virtual void leave() override;
  virtual IntegrationStatus get_status() const override;

  /** ends the current level */
  void finish(bool win = true);
  void respawn(const std::string& sectorname, const std::string& spawnpointname);
  void respawn_with_fade(const std::string& sectorname,
                         const std::string& spawnpointname,
                         const ScreenFade::FadeType fade_type,
                         const Vector &fade_point,
                         const bool make_invincible = false);
  void reset_level();

  void on_player_added(int id);
  bool on_player_removed(int id);

  void set_start_point(const std::string& sector, const std::string& spawnpoint);
  void set_start_pos(const std::string& sector, const Vector& pos);
  inline void set_respawn_point(const std::string& sector, const std::string& spawnpoint)
  {
    m_spawnpoints.push_back({ sector, spawnpoint });
  }
  inline void set_respawn_pos(const std::string& sector, const Vector& pos)
  {
    m_spawnpoints.push_back({ sector, pos });
  }
  void clear_respawn_points();

  inline const SpawnPoint& get_last_spawnpoint() const { return m_spawnpoints.back(); }

  void set_checkpoint_pos(const std::string& sectorname, const Vector& pos);
  inline const SpawnPoint* get_active_checkpoint_spawnpoint() const { return m_activated_checkpoint; }

  inline Sector& get_current_sector() const { return *m_currentsector; }
  inline Level& get_current_level() const { return *m_level; }

  void start_sequence(Player* caller, Sequence seq, const SequenceData* data = nullptr);
  void set_target_timer_paused(bool paused);

  /**
   * returns the "working directory" usually this is the directory where the
   * currently played level resides. This is used when locating additional
   * resources for the current level/world
   */
  std::string get_working_directory() const;
  inline const std::string& get_level_file() const { return m_levelfile; }
  inline bool has_active_sequence() const { return m_end_sequence; }
  void restart_level(bool after_death = false, bool preserve_music = false);

  void toggle_pause();
  void abort_level();
  bool is_active() const;

  inline Savegame& get_savegame() const { return m_savegame; }

  void set_scheduler(SquirrelScheduler& new_scheduler);

private:
  void check_end_conditions();

  void drawstatus(DrawingContext& context);
  void draw_pause(DrawingContext& context);

  void on_escape_press(bool force_quick_respawn);

  Vector get_fade_point(const Vector& position = Vector(0, 0)) const;

public:
  bool reset_button;
  bool reset_checkpoint_button;

  bool m_prevent_death; /**< true if players should enter ghost mode instead of dying */

private:
  std::unique_ptr<Level> m_level;
  SurfacePtr m_statistics_backdrop;

  ssq::Table m_data_table;

  Sector* m_currentsector;

  EndSequence* m_end_sequence;

  bool  m_game_pause;
  float m_speed_before_pause;

  std::string m_levelfile;

  // Spawnpoints
  std::vector<SpawnPoint> m_spawnpoints;
  const SpawnPoint* m_activated_checkpoint;

  // the sector and spawnpoint we should spawn after this frame
  std::string m_newsector;
  std::string m_newspawnpoint;
  ScreenFade::FadeType m_spawn_fade_type;
  Timer m_spawn_fade_timer;
  bool m_spawn_with_invincibility;

  Statistics* m_best_level_statistics;
  Savegame& m_savegame;

  // Note: m_play_time should reset when a level is restarted from the beginning
  //       but NOT if Tux respawns at a checkpoint (for LevelTimes to work)
  float m_play_time; /**< total time in seconds that this session ran interactively */

  bool m_levelintro_shown; /**< true if the LevelIntro screen was already shown */

  int m_coins_at_start; /** How many coins does the player have at the start */
  std::vector<BonusType> m_boni_at_start; /** What boni does the player have at the start */
  std::vector<int> m_max_fire_bullets_at_start; /** How many fire bullets does the player have */
  std::vector<int> m_max_ice_bullets_at_start; /** How many ice bullets does the player have */

  bool m_active; /** Game active? **/

  bool m_end_seq_started;
  bool m_pause_target_timer;

  std::unique_ptr<GameObject> m_current_cutscene_text;

  Timer m_endsequence_timer;

private:
  GameSession(const GameSession&) = delete;
  GameSession& operator=(const GameSession&) = delete;
};

#endif

/* EOF */
