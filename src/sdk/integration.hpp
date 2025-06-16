//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_SDK_INTEGRATION_HPP
#define HEADER_SUPERTUX_SDK_INTEGRATION_HPP

#include "config.h"

#include <memory>
#include <string>
#include <vector>

class IntegrationStatus
{
public:
  IntegrationStatus() :
    m_details(),
    m_party_count(0),
    m_party_max(0),
    m_timestamp(0)
  {
  }

  bool operator !=(const IntegrationStatus& is) const { return !operator==(is); }
  bool operator ==(const IntegrationStatus& is) const {
    if (m_details.size() != is.m_details.size())
      return false;

    if (m_party_count != is.m_party_count ||
        m_party_max != is.m_party_max ||
        m_timestamp != is.m_timestamp)
      return false;

    for (int i = 0; i < static_cast<int>(m_details.size()); i++)
      if (*(m_details.begin() + i) != *(is.m_details.begin() + i))
        return false;

    return true;
  }

  /**
   * A list of lines describing what the player is doing.
   * Should go from general to specific.
   *
   * A good first line should hint other people whether or not
   * the user is available to play with them. For example, stating
   * whether the user is playing online or offline, and if they're
   * playing or if they're in the menus/level select screens.
   *
   * The second line can give more details, such as which level
   * is being played (or edited if in the level editor).
   *
   * The lines should be short (max 100 characters). There shouldn't
   * be more than 3 lines in total. Keep in mind that integrations
   * don't display many lines: Discord displays two; Steam, just one.
   *
   * ================================================================
   *
   * A good example looks like:
   *   Playing (single player)
   *   In level: Welcome to Antartica
   *   Worldmap: Icy island
   *
   * Or:
   *   Racing worldmap (online)
   *   Worldmap: LatestAddon's worldmap [custom]
   *
   * (keep in mind party details have their own variables!)
   *
   * Or even just:
   *   In menu
   *
   */
  std::vector<std::string> m_details;

  /** The amount of people in the group (0 = no party) */
  int m_party_count;

  /** The maximum amount of people in the group (0 = no limit) */
  int m_party_max;

  /**
   *  Any timestamp relative to the game. A value before now
   *  will display an "Elapsed time" counter. A value after
   *  now will display a "Remaining time" counter. A value
   *  of 0 will display no timestamp.
   */
  long int m_timestamp;
};

class Integration
{
public:
  Integration() {}
  virtual ~Integration() {}

  static void setup();
  static void init_all();
  static void update_all();
  static void close_all();

  static void update_status_all(IntegrationStatus status);

public:
  virtual void init() = 0;
  virtual void update() = 0;
  virtual void close() = 0;
  virtual void update_status(IntegrationStatus status) = 0;

private:
  static std::vector<Integration*> sdks;
  static IntegrationStatus current_status;

private:
  Integration(const Integration&) = delete;
  Integration & operator=(const Integration&) = delete;
};

#endif

/* EOF */
