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

#ifndef HEADER_SUPERTUX_SUPERTUX_AUTOTILE_PARSER_HPP
#define HEADER_SUPERTUX_SUPERTUX_AUTOTILE_PARSER_HPP

#include <string>
#include <vector>

#include "supertux/autotile.hpp"

class ReaderMapping;

class AutotileParser final
{
private:
  std::vector<std::unique_ptr<AutotileSet>>& m_autotilesets;
  std::string m_filename;
  std::string m_tiles_path;

  int32_t m_start;
  const int32_t m_end;
  const int32_t m_offset;

public:
  AutotileParser(std::vector<std::unique_ptr<AutotileSet>>& autotilesets, const std::string& filename,
                 int32_t start = 0, int32_t end = 0, int32_t offset = 0);

  void parse();

private:
  void parse_autotileset(const ReaderMapping& reader, bool corner);
  Autotile* parse_autotile(const ReaderMapping& reader, bool corner);
  void parse_mask(const std::string& mask, std::vector<AutotileMask>& autotile_masks, bool solid);
  void parse_mask_corner(const std::string& mask, std::vector<AutotileMask>& autotile_masks);
  void parse_mask(const std::string& mask, std::vector<AutotileMask>& autotile_masks, bool solid, bool is_corner);

private:
  AutotileParser(const AutotileParser&) = delete;
  AutotileParser& operator=(const AutotileParser&) = delete;
};

#endif

/* EOF */
