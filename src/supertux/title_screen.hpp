//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_SUPERTUX_TITLE_SCREEN_HPP
#define HEADER_SUPERTUX_SUPERTUX_TITLE_SCREEN_HPP

#include "supertux/screen.hpp"
#include "util/currenton.hpp"

#include <string>

#include "control/codecontroller.hpp"
#include "supertux/fadetoblack.hpp"
#include "video/surface_ptr.hpp"

class GameSession;
class Savegame;
class Sector;

/** Screen that displays the SuperTux logo, lets players start a new
    game, etc. */
class TitleScreen final : public Screen,
                          public Currenton<TitleScreen>
{
public:
  TitleScreen(Savegame& savegame, bool christmas = false);
  ~TitleScreen() override = default;

  void setup() override;
  void leave() override;

  void draw(Compositor& compositor) override;
  void update(float dt_sec, const Controller& controller) override;

  IntegrationStatus get_status() const override;

  void refresh_level();
  void refresh_copyright_text();

  void set_frame(const std::string& image);

private:
  void setup_sector(Sector& sector);
  void update_level(float dt_sec);

private:
  Savegame& m_savegame;
  const bool m_christmas;

  SurfacePtr m_logo;
  SurfacePtr m_santahat;
  SurfacePtr m_frame;

  std::unique_ptr<CodeController> m_controller;
  std::unique_ptr<GameSession> m_titlesession;

  std::string m_copyright_text;
  std::string m_videosystem_name;

  bool m_jump_was_released;

private:
  TitleScreen(const TitleScreen&) = delete;
  TitleScreen& operator=(const TitleScreen&) = delete;
};

#endif

/* EOF */
