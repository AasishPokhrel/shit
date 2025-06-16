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

#ifndef HEADER_SUPERTUX_UTIL_LINE_ITERATOR_HPP
#define HEADER_SUPERTUX_UTIL_LINE_ITERATOR_HPP

#include <string>

/** Splits a given string at newlines, a new line at the end of the
 *  string is ignored, others are returned as empty lines. Newlines
 *  aren't included in the returned string.Use via:
 *
 *  LineIterator it("Hello\nWorld");
 *  while(it.next())
 *    std::cout << it.get() << std::endl;
 */
class LineIterator final
{
private:
  std::string::const_iterator first;
  std::string::const_iterator last;
  std::string::const_iterator line_end;
  bool has_started;

public:
  LineIterator(const std::string& str);
  LineIterator(std::string::const_iterator first,
               std::string::const_iterator last);

  /** @return false when no characters are left in the string, true
      otherwise */
  bool next();

  /** @return the current line */
  std::string get();
};

#endif

/* EOF */
