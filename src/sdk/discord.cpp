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

#include "config.h"

#ifdef ENABLE_DISCORD

#include "sdk/discord.hpp"

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <discord_rpc.h>


#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"

extern "C" {

  static void handleDiscordReady(const DiscordUser* connectedUser)
  {
    printf("\nDiscord: connected to user %s#%s - %s\n",
           connectedUser->username,
           connectedUser->discriminator,
           connectedUser->userId);
  }

  static void handleDiscordDisconnected(int errcode, const char* message)
  {
    printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
  }

  static void handleDiscordError(int errcode, const char* message)
  {
    printf("\nDiscord: error (%d: %s)\n", errcode, message);
  }

  static void handleDiscordJoin(const char* secret)
  {
    printf("\nDiscord: join (%s)\n", secret);
  }

  static void handleDiscordSpectate(const char* secret)
  {
    printf("\nDiscord: spectate (%s)\n", secret);
  }

  static void handleDiscordJoinRequest(const DiscordUser* request)
  {
    int response = -1;

    printf("\nDiscord: join request from %s#%s - %s\n",
           request->username,
           request->discriminator,
           request->userId);

    response = false ? DISCORD_REPLY_YES : DISCORD_REPLY_NO;

    if (response != -1) {
      Discord_Respond(request->userId, response);
    }
  }

} // extern "C"


DiscordIntegration* DiscordIntegration::driver;

DiscordIntegration::DiscordIntegration() :
  Integration(),
  m_enabled(false)
{
}

DiscordIntegration::~DiscordIntegration()
{
  // It shouldn't get here, but just in case.
  close();
}

DiscordIntegration*
DiscordIntegration::getDriver()
{
  if (!driver)
  {
    driver = new DiscordIntegration();
  }
  return driver;
}

void
DiscordIntegration::init()
{
  if (m_enabled || !g_config->enable_discord) return;

  DiscordEventHandlers handlers;
  memset(&handlers, 0, sizeof(handlers));
  handlers.ready = handleDiscordReady;
  handlers.disconnected = handleDiscordDisconnected;
  handlers.errored = handleDiscordError;
  handlers.joinGame = handleDiscordJoin;
  handlers.spectateGame = handleDiscordSpectate;
  handlers.joinRequest = handleDiscordJoinRequest;
  Discord_Initialize("733576109744062537", &handlers, 1, nullptr);

  log_info << "[Discord] Started" << std::endl;

  m_enabled = true;
}

void
DiscordIntegration::update()
{
  if (!m_enabled) return;

#ifdef DISCORD_DISABLE_IO_THREAD
  Discord_UpdateConnection();
#endif
  Discord_RunCallbacks();
}

void
DiscordIntegration::close()
{
  if (!m_enabled) return;

  Discord_ClearPresence();
  Discord_Shutdown();

  log_info << "[Discord] Closed" << std::endl;

  m_enabled = false;
}

void
DiscordIntegration::update_status(IntegrationStatus status)
{
  if (!m_enabled) return;

  DiscordRichPresence discordPresence;
  memset(&discordPresence, 0, sizeof(discordPresence));

  /*
   *  Possible keys :
   *   state
   *   details
   *   startTimestamp
   *   endTimestamp
   *   largeImageKey
   *   smallImageKey
   *   partyId
   *   partySize
   *   partyMax
   *   matchSecret
   *   joinSecret
   *   spectateSecret
   *   instance
   */

  if (status.m_details.size() >= 1)
    discordPresence.state = status.m_details.begin()->c_str();

  if (status.m_details.size() >= 2)
    discordPresence.details = (status.m_details.begin() + 1)->c_str();

  if (status.m_timestamp != 0) {
    if (status.m_timestamp > time(nullptr)) {
      discordPresence.endTimestamp = status.m_timestamp;
    } else {
      discordPresence.startTimestamp = status.m_timestamp;
    }
  }

  // TODO: Manage parties and all.

  discordPresence.largeImageKey = "supertux_logo";
  Discord_UpdatePresence(&discordPresence);
}

#endif

/* EOF */
