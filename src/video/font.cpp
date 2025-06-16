//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                     Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/font.hpp"

std::string
Font::wrap_to_chars(const std::string& s, int line_length, std::string* overflow)
{
  // If text is already smaller, return full text.
  if (static_cast<int>(s.length()) <= line_length) {
    if (overflow) *overflow = "";
    return s;
  }

  // If we can find a whitespace character to break at, return text up to this character.
  int i = line_length;
  while ((i > 0) && (s[i] != ' ')) i--;
  if (i > 0) {
    if (overflow) *overflow = s.substr(i+1);
    return s.substr(0, i);
  }

  // FIXME: Wrap at line_length, taking care of multibyte characters.
  if (overflow) *overflow = "";
  return s;
}

/* EOF */
