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

#include "supertux/sequence.hpp"
#include "util/log.hpp"

Sequence string_to_sequence(const std::string& sequencename) {
  if (sequencename == "endsequence")
    return SEQ_ENDSEQUENCE;
  if (sequencename == "stoptux")
    return SEQ_STOPTUX;
  if (sequencename == "fireworks")
    return SEQ_FIREWORKS;

  log_warning << "Unknown sequence: " << sequencename << std::endl;
  return SEQ_ENDSEQUENCE;
}

std::string sequence_to_string(const Sequence& seq) {
  switch (seq) {
    case SEQ_ENDSEQUENCE:
      return "endsequence";
    case SEQ_STOPTUX:
      return "stoptux";
    case SEQ_FIREWORKS:
      return "fireworks";
    default:
      return "unknown sequence " + std::to_string( static_cast<int>(seq) );
  }
}

/* EOF */
