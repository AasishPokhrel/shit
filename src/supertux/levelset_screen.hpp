//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVELSET_SCREEN_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVELSET_SCREEN_HPP

#include <optional>
#include <string>

#include "math/vector.hpp"
#include "supertux/screen.hpp"
#include "util/currenton.hpp"

class Savegame;

class LevelsetScreen final : public Screen,
                             public Currenton<LevelsetScreen>
{
private:
  std::string m_basedir;
  std::string m_level_filename;
  Savegame& m_savegame;
  bool m_level_started;
  bool m_solved;

public:
  LevelsetScreen(const std::string& basedir, const std::string& level_filename, Savegame& savegame,
                 const std::optional<std::pair<std::string, Vector>>& start_pos);

  virtual void draw(Compositor& compositor) override;
  virtual void update(float dt_sec, const Controller& controller) override;

  virtual void setup() override;
  virtual void leave() override;

  virtual IntegrationStatus get_status() const override;

  void finished_level(bool win);

private:
  std::optional<std::pair<std::string, Vector>> m_start_pos;

  LevelsetScreen(const LevelsetScreen&) = delete;
  LevelsetScreen& operator=(const LevelsetScreen&) = delete;
};

#endif

/* EOF */
