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

#include "supertux/player_status.hpp"

#include <sstream>

#include "audio/sound_manager.hpp"
#include "object/player.hpp"
#include "supertux/globals.hpp"
#include "supertux/game_session.hpp"
#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "worldmap/level_tile.hpp"
#include "worldmap/worldmap.hpp"

static const int START_COINS = 100;
static const int MAX_COINS = 9999;

PlayerStatus::PlayerStatus(int num_players) :
  m_num_players(num_players),
  m_item_pockets(num_players),
  m_override_item_pocket(Level::INHERIT),
  coins(START_COINS),
  bonus(num_players),
  worldmap_sprite("images/worldmap/common/tux.sprite"),
  last_worldmap(),
  title_level()
{
  reset(num_players);

  // FIXME: Move sound handling into PlayerStatusHUD
  if (SoundManager::current()) {
    SoundManager::current()->preload("sounds/coin.wav");
    SoundManager::current()->preload("sounds/lifeup.wav");
  }
}

void
PlayerStatus::take_checkpoint_coins()
{
  int subtract_value = std::max(coins / 10, 25);
  if (coins - subtract_value >= 0)
    coins -= subtract_value;
  else
    coins = 0;
}

void
PlayerStatus::reset(int num_players)
{
  coins = START_COINS;

  // Keep in sync with a section in read()
  bonus.clear();
  bonus.resize(num_players, BONUS_NONE);
  m_item_pockets.clear();
  m_item_pockets.resize(num_players, BONUS_NONE);

  m_num_players = num_players;
}

int
PlayerStatus::get_max_coins() const
{
  return MAX_COINS;
}

bool
PlayerStatus::can_reach_checkpoint() const
{
  return GameSession::current()->get_active_checkpoint_spawnpoint();
}

bool
PlayerStatus::respawns_at_checkpoint() const
{
  return GameSession::current()->get_last_spawnpoint().is_checkpoint ||
         GameSession::current()->reset_checkpoint_button;
}

std::string
PlayerStatus::get_bonus_name(BonusType bonustype)
{
  switch (bonustype) {
    case BONUS_FIRE:
      return "fireflower";
    case BONUS_ICE:
      return "iceflower";
    case BONUS_AIR:
      return "airflower";
    case BONUS_EARTH:
      return "earthflower";
    case BONUS_GROWUP:
      return "egg";
    case BONUS_NONE:
      return "none";
    default:
      log_warning << "Unknown bonus " << static_cast<int>(bonustype) << std::endl;
      return "none";
  }
}

BonusType
PlayerStatus::get_bonus_from_name(const std::string& name)
{
  if (name == "none") {
    return BONUS_NONE;
  } else if (name == "growup") {
    return BONUS_GROWUP;
  } else if (name == "fireflower") {
    return BONUS_FIRE;
  } else if (name == "iceflower") {
    return BONUS_ICE;
  } else if (name == "airflower") {
    return BONUS_AIR;
  } else if (name == "earthflower") {
    return BONUS_EARTH;
  } else {
    log_warning << "Unknown bonus '" << name << "' in savefile"<< std::endl;
    return BONUS_NONE;
  }
}

std::string
PlayerStatus::get_bonus_sprite(BonusType bonustype)
{
  switch (bonustype) {
    case BONUS_FIRE:
      return "images/powerups/fireflower/fireflower.sprite";
    case BONUS_ICE:
      return "images/powerups/iceflower/iceflower.sprite";
    case BONUS_AIR:
      return "images/powerups/airflower/airflower.sprite";
    case BONUS_EARTH:
      return "images/powerups/earthflower/earthflower.sprite";
    case BONUS_GROWUP:
      return "images/powerups/egg/egg.sprite";
    default:
      return "";
  }
}

void
PlayerStatus::add_coins(int count, bool play_sound)
{
  coins = std::min(coins + count, MAX_COINS);

  if (!play_sound)
    return;

  static float sound_played_time = 0;
  if (count >= 100)
    SoundManager::current()->play("sounds/lifeup.wav");
  else if (g_real_time > sound_played_time + 0.010f) {
    SoundManager::current()->play("sounds/coin.wav");
    sound_played_time = g_real_time;
  }
}

void
PlayerStatus::write(Writer& writer)
{
  writer.write("num_players", m_num_players);

  for (int i = 0; i < m_num_players; i++)
  {
    if (i != 0)
    {
      writer.start_list("tux" + std::to_string(i + 1));
    }

    writer.write("bonus", get_bonus_name(bonus[i]));
    writer.write("item-pocket", get_bonus_name(m_item_pockets[i]));

    if (i != 0)
    {
      writer.end_list("tux" + std::to_string(i + 1));
    }
  }

  writer.write("coins", coins);

  writer.write("worldmap-sprite", worldmap_sprite, false);
  writer.write("last-worldmap", last_worldmap, false);
  writer.write("title-level", title_level);
}

void
PlayerStatus::read(const ReaderMapping& mapping)
{
  int num_players_in_file = 1;
  mapping.get("num_players", num_players_in_file);

  reset(std::max(m_num_players, num_players_in_file));

  auto iter = mapping.get_iter();

  while (iter.next())
  {
    try
    {
      if (iter.get_key().size() > 3 && iter.get_key().substr(0, 3) == "tux")
      {
        int id = std::stoi(iter.get_key().substr(3)) - 1;

        if (id >= m_num_players)
        {
          log_warning << "ID larger than amount of players when reading player state: " << id << std::endl;

          // Keep this in sync with reset()
          if (bonus.size() < static_cast<size_t>(id))
            bonus.resize(id, BONUS_NONE);

          if (m_item_pockets.size() < static_cast<size_t>(id))
            m_item_pockets.resize(id, BONUS_NONE);
        }
        else if (id == 0)
        {
          log_warning << "Refusing to parse player 1 when reading player state,"
                         "please don't put player 1 data in a (tux1 ...)"
                         "wrapper for retrocompatibiility" << std::endl;
        }

        auto map = iter.as_mapping();
        parse_bonus_mapping(map, id);
      }
    }
    catch (const std::exception& e)
    {
      log_warning << "Couldn't parse player from player status save: " << e.what() << std::endl;
    }
  }

  parse_bonus_mapping(mapping, 0);

  mapping.get("coins", coins);

  mapping.get("worldmap-sprite", worldmap_sprite);
  mapping.get("last-worldmap", last_worldmap);
  mapping.get("title-level", title_level);
}

void
PlayerStatus::give_item_from_pocket(Player* player)
{
  if (!is_item_pocket_allowed())
    return;

  BonusType bonustype = m_item_pockets[player->get_id()];
  if (bonustype == BONUS_NONE)
    return;

  m_item_pockets[player->get_id()] = BONUS_NONE;

  auto& powerup = Sector::get().add<PocketPowerUp>(bonustype, Vector(0,0));
  powerup.set_pos(player->get_pos() - Vector(0.f, powerup.get_bbox().get_height() + 15));
}

void
PlayerStatus::add_item_to_pocket(BonusType bonustype, Player* player)
{
  if (!is_item_pocket_allowed())
    return;

  if (bonustype <= BONUS_GROWUP)
    return;

  m_item_pockets[player->get_id()] = bonustype;
}

BonusType
PlayerStatus::get_item_pocket(const Player* player) const
{
  return m_item_pockets[player->get_id()];
}

bool
PlayerStatus::is_item_pocket_allowed() const
{
  if (m_override_item_pocket != Level::INHERIT)
  {
    return m_override_item_pocket == Level::ON;
  }

  GameSession* session = GameSession::current();
  if (!session)
  {
    worldmap::WorldMap* worldmap = worldmap::WorldMap::current();
    if (worldmap)
    {
      return worldmap->is_item_pocket_allowed();
    }
    else
    {
      // Not in a level nor in a worldmap. Return true, I guess.
      return true;
    }
  }

  Level& level = session->get_current_level();
  int allowed = static_cast<Level::Setting>(level.m_allow_item_pocket);

  if (allowed != Level::INHERIT)
  {
    return allowed == Level::ON;
  }
  else
  {
    worldmap::WorldMap* worldmap = worldmap::WorldMap::current();
    if (worldmap)
    {
      return worldmap->is_item_pocket_allowed();
    }
    else
    {
      // This level is probably in a levelset, pick ON.
      return true;
    }
  }
}

void
PlayerStatus::parse_bonus_mapping(const ReaderMapping& map, int id)
{
  std::string bonusname;
  if (map.get("bonus", bonusname)) {
    bonus[id] = get_bonus_from_name(bonusname);
  }

  if (map.get("item-pocket", bonusname)) {
    m_item_pockets[id] = get_bonus_from_name(bonusname);
  }
}

std::string
PlayerStatus::get_bonus_prefix(int player_id) const
{
  switch (bonus[player_id]) {
    default:
    case BONUS_NONE:
      return "small";
    case BONUS_GROWUP:
      return "big";
    case BONUS_FIRE:
      return "fire";
    case BONUS_ICE:
      return "ice";
    case BONUS_AIR:
      return "air";
    case BONUS_EARTH:
      return "earth";
  }
}

void
PlayerStatus::add_player()
{
  m_num_players++;

  bonus.resize(m_num_players, BONUS_NONE);
  m_item_pockets.resize(m_num_players, BONUS_NONE);
}

void
PlayerStatus::remove_player(int player_id)
{
  m_num_players--;

  for (int i = player_id; i < m_num_players; i++)
  {
    bonus[i] = bonus[i + 1];
    m_item_pockets[i] = m_item_pockets[i + 1];
  }

  bonus.resize(m_num_players, BONUS_NONE);
  m_item_pockets.resize(m_num_players, BONUS_NONE);
}


PlayerStatus::PocketPowerUp::PocketPowerUp(BonusType bonustype, Vector pos):
  PowerUp(pos, PowerUp::get_type_from_bonustype(bonustype)),
  m_cooldown_timer(),
  m_blink_timer(),
  m_visible(true)
{
  physic.set_velocity_y(-325.f);
  physic.set_gravity_modifier(0.4f);
  set_layer(LAYER_FOREGROUND1);
  m_col.m_group = COLGROUP_DISABLED;
}

void
PlayerStatus::PocketPowerUp::update(float dt_sec)
{
  PowerUp::update(dt_sec);

  bool check = m_cooldown_timer.check();
  if (!m_cooldown_timer.started() && !check && m_col.m_group != COLGROUP_TOUCHABLE)
  {
    m_cooldown_timer.start(1.3f);
    m_blink_timer.start(.15f, true);
  }

  if (check)
  {
    m_visible = true;
    m_blink_timer.stop();
    m_col.m_group = COLGROUP_TOUCHABLE;
  }

  if (m_blink_timer.check())
    m_visible = !m_visible;
}

void
PlayerStatus::PocketPowerUp::draw(DrawingContext& context)
{
  if (!m_visible)
    return;

  PowerUp::draw(context);
}

/* EOF */
