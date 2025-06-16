//  SuperTux
//  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_GAME_MANAGER_HPP
#define HEADER_SUPERTUX_SUPERTUX_GAME_MANAGER_HPP

#include "util/currenton.hpp"

#include <memory>
#include <optional>
#include <string>

#include "math/vector.hpp"

class Savegame;
class World;

class GameManager final : public Currenton<GameManager>
{
public:
  GameManager();

  bool start_worldmap(const World& world, const std::string& worldmap_filename = "",
                      const std::string& sector = "", const std::string& spawnpoint = "");
  bool start_worldmap(const World& world, const std::string& worldmap_filename,
                      const std::optional<std::pair<std::string, Vector>>& start_pos);
  void start_level(const World& world, const std::string& level_filename,
                   const std::optional<std::pair<std::string, Vector>>& start_pos = std::nullopt);

private:
  std::unique_ptr<Savegame> m_savegame;

private:
  GameManager(const GameManager&) = delete;
  GameManager& operator=(const GameManager&) = delete;
};

#endif

/* EOF */
