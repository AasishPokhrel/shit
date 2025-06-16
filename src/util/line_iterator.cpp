// Pingus - A free Lemmings clone
// Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "util/line_iterator.hpp"

LineIterator::LineIterator(const std::string& str)
  : first(str.begin()),
    last(str.end()),
    line_end(str.begin()),
    has_started(false)
{
}

LineIterator::LineIterator(std::string::const_iterator first_,
                           std::string::const_iterator last_)
  : first(first_),
    last(last_),
    line_end(first_),
    has_started(false)
{
}

bool
LineIterator::next()
{
  if (line_end == last || (line_end+1 == last && *first == '\n'))
  {
    return false;
  }
  else
  {
    if (has_started)
      first = line_end + 1;

    do {
      ++line_end;
    } while(line_end != last && *line_end != '\n');

    has_started = true;
    return true;
  }
}

std::string
LineIterator::get()
{
  return std::string(first, line_end);
}

/* EOF */
