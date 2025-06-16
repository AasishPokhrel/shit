//  SuperTux
//  Copyright (C) 2003 Tobias Glaesser <tobi.web@gmx.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_PLAYER_STATUS_HPP
#define HEADER_SUPERTUX_SUPERTUX_PLAYER_STATUS_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "object/powerup.hpp"
#include "supertux/timer.hpp"
#include "supertux/level.hpp"

class DrawingContext;
class Player;
class ReaderMapping;
class Writer;

static const float BORDER_X = 10;
static const float BORDER_Y = 10;

/**
 * @scripting
 */
enum BonusType {
  BONUS_NONE = 0, /*!< @description No bonus. */
  BONUS_GROWUP,   /*!< @description Growup (a.k.a. egg) bonus. */
  BONUS_FIRE,     /*!< @description Fire bonus. */
  BONUS_ICE,      /*!< @description Ice bonus. */
  BONUS_AIR,      /*!< @description Air bonus. */
  BONUS_EARTH     /*!< @description Earth bonus. */
};

/** This class keeps player status between different game sessions (for
    example when switching maps in the worldmap) */
class PlayerStatus final
{
public:
  PlayerStatus(int num_players);
  void reset(int num_players);
  void add_coins(int count, bool play_sound = true);
  void take_checkpoint_coins();

  void write(Writer& writer);
  void read(const ReaderMapping& mapping);

  void give_item_from_pocket(Player* player);
  void add_item_to_pocket(BonusType bonustype, Player* player);
  BonusType get_item_pocket(const Player* player) const;

  bool is_item_pocket_allowed() const;

  int get_max_coins() const;
  bool can_reach_checkpoint() const;
  bool respawns_at_checkpoint() const;
  bool has_hat_sprite(int player_id) const { return bonus[player_id] > BONUS_GROWUP; }

  static std::string get_bonus_name(BonusType bonustype);
  static BonusType get_bonus_from_name(const std::string& name);

  static std::string get_bonus_sprite(BonusType bonustype);
  std::string get_bonus_prefix(int player_id) const;/**Returns the prefix of the animations that should be displayed*/

  void add_player();
  void remove_player(int player_id);

private:
  void parse_bonus_mapping(const ReaderMapping& map, int id);

private:
  /// PowerUp that flings itself upwards
  /// can't be collected right away.
  class PocketPowerUp : public PowerUp
  {
  public:
    PocketPowerUp(BonusType bonustype, Vector pos);
    virtual void update(float dt_sec) override;
    virtual void draw(DrawingContext& context) override;

  public:
    Timer m_cooldown_timer;
    Timer m_blink_timer;
    bool m_visible;

  private:
    PocketPowerUp(const PocketPowerUp&) = delete;
    PocketPowerUp& operator=(const PocketPowerUp&) = delete;
  };

public:
  int m_num_players;

  std::vector<BonusType> m_item_pockets;
  Level::Setting m_override_item_pocket;

  int coins;
  std::vector<BonusType> bonus;

  std::string worldmap_sprite; /**< the sprite of Tux that should be used in worldmap */
  std::string last_worldmap; /**< the last played worldmap */
  std::string title_level; /**< level to be used for the title screen, overrides the value of the same property for the world */

private:
  PlayerStatus(const PlayerStatus&) = delete;
  PlayerStatus& operator=(const PlayerStatus&) = delete;
};

#endif

/* EOF */
