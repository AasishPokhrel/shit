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

#include "sdk/integration.hpp"

#include <vector>

#ifdef ENABLE_DISCORD
#include "sdk/discord.hpp"
#endif
#include "util/log.hpp"

std::vector<Integration*> Integration::sdks;
IntegrationStatus Integration::current_status;

void
Integration::setup()
{
#ifdef ENABLE_DISCORD
  sdks.push_back(DiscordIntegration::getDriver());
#endif
}

void
Integration::init_all()
{
  for (Integration* sdk : sdks)
    sdk->init();
}

void
Integration::update_all()
{
  for (Integration* sdk : sdks)
    sdk->update();
}

void
Integration::close_all()
{
  for (Integration* sdk : sdks)
    sdk->close();
}

void
Integration::update_status_all(IntegrationStatus status)
{
  if (current_status == status)
    return;

  current_status = status;

  if (status.m_details.size() > 0)
    log_info << "Setting status: " << *status.m_details.begin() << std::endl;

  for (Integration* sdk : sdks)
    sdk->update_status(status);
}

