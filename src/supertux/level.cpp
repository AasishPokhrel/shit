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

#include "supertux/level.hpp"

#include <numeric>

#include <physfs.h>

#include "editor/editor.hpp"
#include "object/player.hpp"
#include "physfs/util.hpp"
#include "supertux/game_session.hpp"
#include "supertux/player_status_hud.hpp"
#include "supertux/savegame.hpp"
#include "supertux/sector.hpp"
#include "trigger/secretarea_trigger.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/string_util.hpp"
#include "util/writer.hpp"

static PlayerStatus s_dummy_player_status(1);
Level* Level::s_current = nullptr;

Level::Level(bool worldmap) :
  m_is_worldmap(worldmap),
  m_name("noname"),
  m_author("SuperTux Player"),
  m_contact(),
  m_license(),
  m_filename(),
  m_note(),
  m_sectors(),
  m_stats(),
  m_target_time(),
  m_tileset("images/tiles.strf"),
  m_allow_item_pocket(ON),
  m_suppress_pause_menu(),
  m_is_in_cutscene(false),
  m_skip_cutscene(false),
  m_icon(),
  m_icon_locked(),
  m_wmselect_bkg()
{
  s_current = this;

  if (!is_worldmap())
  {
    m_allow_item_pocket = INHERIT;
  }
}

Level::~Level()
{
  m_sectors.clear();
}

void
Level::initialize()
{
  if (m_sectors.empty())
    throw std::runtime_error("Level has no sectors!");

  m_stats.init(*this);

  Savegame* savegame = (GameSession::current() && !Editor::is_active() ?
    &GameSession::current()->get_savegame() : nullptr);
  PlayerStatus& player_status = savegame ? savegame->get_player_status() : s_dummy_player_status;

  // Condition 1: If there is a savegame, it shouldn't be from the title screen. (Don't load HUD on title screen)
  // Condition 2: Pause menu shouldn't be suppressed.
  // Condition 3: The level shouldn't be loaded in the editor.
  if ((!savegame || !savegame->is_title_screen()) &&
      !m_suppress_pause_menu && !Editor::is_active())
  {
    for (auto& sector : m_sectors)
      sector->add<PlayerStatusHUD>(player_status);
  }

  // All players will be added to the first sector. They are moved between sectors.
  Sector* sector = m_sectors.at(0).get();
  sector->add<Player>(player_status, "Tux", 0);

  if (savegame && !savegame->is_title_screen())
  {
    for (int id = 1; id < InputManager::current()->get_num_users() || id == 0; id++)
    {
      if (!InputManager::current()->has_corresponsing_controller(id)
          && !InputManager::current()->m_uses_keyboard[id])
        continue;

      s_dummy_player_status.add_player();

      sector->add<Player>(player_status, "Tux" + std::to_string(id + 1), id);
    }
  }
  sector->flush_game_objects();
}

void
Level::save(std::ostream& stream)
{
  Writer writer(stream);
  save(writer);
}

void
Level::save(const std::string& filepath, bool retry)
{
  //FIXME: It tests for directory in supertux/data, but saves into .supertux2.
  try {
    { // make sure the level directory exists
      std::string dirname = FileSystem::dirname(filepath);
      if (!PHYSFS_exists(dirname.c_str()))
      {
        if (!PHYSFS_mkdir(dirname.c_str()))
        {
          std::ostringstream msg;
          msg << "Couldn't create directory for level '"
              << dirname << "': " <<physfsutil::get_last_error();
          throw std::runtime_error(msg.str());
        }
      }

      if (!physfsutil::is_directory(dirname))
      {
        std::ostringstream msg;
        msg << "Level path '" << dirname << "' is not a directory";
        throw std::runtime_error(msg.str());
      }
    }

    Writer writer(filepath);
    save(writer);
    log_info << "Level saved as " << filepath << "."
             << (StringUtil::has_suffix(filepath, "~") ? " [Autosave]" : "")
             << std::endl;
  } catch(std::exception& e) {
    if (retry) {
      std::stringstream msg;
      msg << "Problem when saving level '" << filepath << "': " << e.what();
      throw std::runtime_error(msg.str());
    } else {
      log_warning << "Failed to save the level, retrying..." << std::endl;
      { // create the level directory again
        std::string dirname = FileSystem::dirname(filepath);
        if (!PHYSFS_mkdir(dirname.c_str()))
        {
          std::ostringstream msg;
          msg << "Couldn't create directory for level '"
              << dirname << "': " <<physfsutil::get_last_error();
          throw std::runtime_error(msg.str());
        }
      }
      save(filepath, true);
    }
  }
}

void
Level::save(Writer& writer)
{
  writer.start_list("supertux-level");
  // Starts writing to supertux level file. Keep this at the very beginning.

  writer.write("version", 3);
  writer.write("name", m_name, true);
  writer.write("author", m_author, false);
  if (!m_note.empty()) {
    writer.write("note", m_note, true);
  }
  if (!m_contact.empty()) {
    writer.write("contact", m_contact, false);
  }
  if (!m_license.empty()) {
    writer.write("license", m_license, false);
  }
  if (m_target_time != 0.0f){
    writer.write("target-time", m_target_time);
  }
  if(m_suppress_pause_menu) {
    writer.write("suppress-pause-menu", m_suppress_pause_menu);
  }

  writer.write("allow-item-pocket", get_setting_name(static_cast<Level::Setting>(m_allow_item_pocket)));

  writer.write("icon", m_icon);
  writer.write("icon-locked", m_icon_locked);

  if (!m_wmselect_bkg.empty())
    writer.write("bkg", m_wmselect_bkg);

  if (!m_is_worldmap)
  {
    writer.start_list("statistics");
    m_stats.get_preferences().write(writer);
    writer.end_list("statistics");
  }

  for (auto& sector : m_sectors) {
    sector->save(writer);
  }

  if (m_tileset != "images/tiles.strf")
    writer.write("tileset", m_tileset, false);

  // Ends writing to supertux level file. Keep this at the very end.
  writer.end_list("supertux-level");
}

std::string
Level::get_setting_name(Setting setting)
{
  switch (setting)
  {
    case ON:
      return "on";

    case OFF:
      return "off";

    case INHERIT:
      return "inherit";
  }

  return "on";
}

Level::Setting
Level::get_setting_from_name(std::string setting)
{
  if (setting == "on")
  {
    return ON;
  }
  else if (setting == "off")
  {
    return OFF;
  }
  else if (setting == "inherit")
  {
    return INHERIT;
  }

  return ON;
}

void
Level::add_sector(std::unique_ptr<Sector> sector)
{
  Sector* test = get_sector(sector->get_name());
  if (test != nullptr) {
    throw std::runtime_error("Trying to add 2 sectors with same name");
  } else {
    m_sectors.push_back(std::move(sector));
  }
}

Sector*
Level::get_sector(const std::string& name_) const
{
  auto _sector = std::find_if(m_sectors.begin(), m_sectors.end(), [name_] (const std::unique_ptr<Sector>& sector) {
    return sector->get_name() == name_;
  });
  if(_sector == m_sectors.end())
    return nullptr;
  return _sector->get();
}

size_t
Level::get_sector_count() const
{
  return m_sectors.size();
}

Sector*
Level::get_sector(size_t num) const
{
  return m_sectors.at(num).get();
}

int
Level::get_total_coins() const
{
  int total_coins = 0;
  for (auto const& sector : m_sectors)
  {
    for (const auto& obj : sector->get_objects())
      total_coins += obj->get_coins_worth();
  }
  return total_coins;
}

int
Level::get_total_badguys() const
{
  int total_badguys = 0;
  for (auto const& sector : m_sectors) {
    total_badguys += sector->get_object_count<BadGuy>([] (const BadGuy& badguy) {
      return badguy.m_countMe;
    });
  }
  return total_badguys;
}

int
Level::get_total_secrets() const
{
  auto get_secret_count = [](int accumulator, const std::unique_ptr<Sector>& sector) {
    return accumulator + sector->get_object_count<SecretAreaTrigger>();
  };
  return std::accumulate(m_sectors.begin(), m_sectors.end(), 0, get_secret_count);
}

std::vector<Player*>
Level::get_players() const
{
  std::vector<Player*> players;
  for (const auto& sector : m_sectors)
    for (auto& player : sector->get_objects_by_type<Player>())
      players.push_back(&player);

  return players;
}

void
Level::reactivate()
{
  s_current = this;
}

/* EOF */
