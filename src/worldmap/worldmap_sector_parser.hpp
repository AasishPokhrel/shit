//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#ifndef HEADER_SUPERTUX_WORLDMAP_WORLDMAP_SECTOR_PARSER_HPP
#define HEADER_SUPERTUX_WORLDMAP_WORLDMAP_SECTOR_PARSER_HPP

#include "supertux/sector_parser.hpp"

namespace worldmap {

class WorldMap;
class WorldMapSector;

class WorldMapSectorParser final : public SectorParser
{
public:
  static std::unique_ptr<WorldMapSector> from_reader(WorldMap& worldmap, const ReaderMapping& sector);

private:
  WorldMapSectorParser(WorldMapSector& sector);

private:
  WorldMapSector& get_sector() const;

  bool parse_object_additional(const std::string& name, const ReaderMapping& reader) override;

private:
  WorldMapSectorParser(const WorldMapSectorParser&) = delete;
  WorldMapSectorParser& operator=(const WorldMapSectorParser&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
