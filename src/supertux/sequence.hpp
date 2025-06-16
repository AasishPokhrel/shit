//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_SEQUENCE_HPP
#define HEADER_SUPERTUX_SUPERTUX_SEQUENCE_HPP

#include <string>

enum Sequence {
  SEQ_ENDSEQUENCE,
  SEQ_STOPTUX,
  SEQ_FIREWORKS
};

enum TilemapFadeType {
  FADE_IN = 0,
  FADE_OUT = 1
};

struct SequenceData {
  SequenceData(const std::string& spawnpoint_, const std::string& fade_tilemap_, TilemapFadeType& fade_type_) :
    spawnpoint(spawnpoint_),
    fade_tilemap(fade_tilemap_),
    fade_type(fade_type_)
  {
  }
  const std::string& spawnpoint;
  const std::string& fade_tilemap;
  const TilemapFadeType& fade_type;
};

Sequence string_to_sequence(const std::string& sequencename);
std::string sequence_to_string(const Sequence& seq);

#endif
/* EOF */
