//  SuperTux (Statistics module)
//  Copyright (C) 2004 Ricardo Cruz <rick2@aeiou.pt>
//  Copyright (C) 2006 Ondrej Hosek <ondra.hosek@gmail.com>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
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

#ifndef HEADER_SUPERTUX_SUPERTUX_STATISTICS_HPP
#define HEADER_SUPERTUX_SUPERTUX_STATISTICS_HPP

#include "video/color.hpp"
#include "video/surface_ptr.hpp"

class DrawingContext;
class Level;
class Menu;
class ReaderMapping;
class Writer;

namespace ssq {
class Table;
} // namespace ssq

/** This class is a layer between level and worldmap to keep
    track of stuff like scores, and minor, but funny things, like
    number of jumps and stuff */
class Statistics final
{
private:
  static Color header_color;
  static Color text_color;
  static Color perfect_color;

public:
  static std::string coins_to_string(int coins, int total_coins);
  static std::string frags_to_string(int badguys, int total_badguys);
  static std::string secrets_to_string(int secrets, int total_secrets);
  static std::string time_to_string(float time);

public:
  class Preferences final
  {
  public:
    Preferences();

    void parse(const ReaderMapping& reader);
    void write(Writer& writer) const;

    void add_to_menu(Menu& menu);

  public:
    bool enable_coins;
    bool enable_badguys;
    bool enable_secrets;
  };

public:
  enum Status { INVALID, ACCUMULATING, FINAL };

public:
  Statistics(); /**< Creates new statistics, call reset() before counting */

  /** serialize statistics object as squirrel table "statistics" */
  void serialize_to_squirrel(ssq::Table& table) const;

  /** unserialize statistics object from squirrel table "statistics" */
  void unserialize_from_squirrel(const ssq::Table& table);

  inline Preferences& get_preferences() { return m_preferences; }
  inline const Preferences& get_preferences() const { return m_preferences; }
  void add_preferences_to_menu(Menu& menu);

  void draw_worldmap_info(DrawingContext& context, float target_time); /**< draw worldmap stat HUD */
  void draw_endseq_panel(DrawingContext& context, Statistics* best_stats, const SurfacePtr& backdrop, float target_time); /**< draw panel shown during level's end sequence */
  void draw_ingame_stats(DrawingContext& context, bool on_pause_menu); /**< draw in-game stats */

  /** Updates the timers for in-game stats rendering. Should be used from the same object that calls draw_ingame_stats(). */
  void update_timers(float dt_sec);

  void init(const Level& level);
  void finish(float time);
  void invalidate();

  void update(const Statistics& stats); /**< Given another Statistics object finds the best of each one */
  bool completed(const float target_time) const; /* Check if stats match total stats */

  inline int get_coins() const { return m_coins; }
  inline int get_badguys() const { return m_badguys; }
  inline int get_secrets() const { return m_secrets; }
  inline float get_time() const { return m_time; }
  inline Status get_status() const { return m_status; }

  void increment_coins() { m_coins++; check_coins(); }
  void increment_badguys() { m_badguys++; check_badguys(); }
  void increment_secrets() { m_secrets++; check_secrets(); }

private:
  void calculate_max_caption_length();

  void check_coins();
  void check_badguys();
  void check_secrets();

private:
  enum Status m_status;

public:
  int m_total_coins; /**< coins in level */
  int m_total_badguys; /**< (vincible) badguys in level */
  int m_total_secrets; /**< secret areas in level */

private:
  int m_coins; /**< coins collected */
  int m_badguys; /**< badguys actively killed */
  int m_secrets; /**< secret areas found */

  float m_time; /**< seconds needed */

  bool m_cleared_coins,
       m_cleared_badguys,
       m_cleared_secrets;

  float m_coins_time,
        m_badguys_time,
        m_secrets_time;

  Preferences m_preferences;

private:
  int m_max_width; /** < Gets the max width of a stats line, 255 by default */

  /** Captions */
  std::string CAPTION_MAX_COINS;
  std::string CAPTION_MAX_FRAGGING;
  std::string CAPTION_MAX_SECRETS;
  std::string CAPTION_BEST_TIME;
  std::string CAPTION_TARGET_TIME;

  float WMAP_INFO_LEFT_X;
  float WMAP_INFO_RIGHT_X;
  float WMAP_INFO_TOP_Y1;
  float WMAP_INFO_TOP_Y2;

  SurfacePtr coin_icon;
  SurfacePtr badguy_icon;
  SurfacePtr secret_icon;

private:
  Statistics(const Statistics&) = delete;
  Statistics& operator=(const Statistics&) = delete;
};

#endif

/* EOF */
