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

#ifndef HEADER_SUPERTUX_SDK_DISCORD_HPP
#define HEADER_SUPERTUX_SDK_DISCORD_HPP

#include "config.h"

#ifdef ENABLE_DISCORD

#include "sdk/integration.hpp"

class DiscordIntegration final : public Integration
{
public:
  static DiscordIntegration* getDriver();

public:
  virtual void init() override;
  virtual void update() override;
  virtual void close() override;
  virtual void update_status(IntegrationStatus status) override;

protected:
  DiscordIntegration();
  ~DiscordIntegration() override;

private:
  static DiscordIntegration* driver;
  bool m_enabled;

private:
  DiscordIntegration(const DiscordIntegration&) = delete;
  DiscordIntegration & operator=(const DiscordIntegration&) = delete;
};

#endif

#endif

/* EOF */
