//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "string_util.hpp"

#include <algorithm>
#include <string>
#include <sstream>
#include <string.h>

bool
StringUtil::has_suffix(const std::string& data, const std::string& suffix)
{
  return data.length() >= suffix.length()
         && data.compare(data.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool
StringUtil::numeric_less(const std::string& lhs, const std::string& rhs)
{
  std::string::size_type i = 0;
  std::string::size_type min_len = std::min(lhs.size(), rhs.size());

  while (i < min_len)
  {
    if (isdigit(lhs[i]) && isdigit(rhs[i]))
    {
      // have two digits, so check which number is smaller
      std::string::size_type li = i+1;
      std::string::size_type ri = i+1;

      // find the end of the number in both strings
      while (li < lhs.size() && isdigit(lhs[li])) { li += 1; }
      while (ri < rhs.size() && isdigit(rhs[ri])) { ri += 1; }

      if (li == ri)
      {
        // end is at the same point in both strings, so do a detaile
        // comparism of the numbers
        for (std::string::size_type j = i; j < li; ++j)
        {
          if (lhs[j] != rhs[j])
          {
            return lhs[j] < rhs[j];
          }
        }

        // numbers are the same, so jump to the end of the number and compare
        i = li;
      }
      else
      {
        // numbers have different numbers of digits, so the number
        // with the least digits wins
        return li < ri;
      }
    }
    else
    {
      // do normal character comparism
      if (lhs[i] != rhs[i])
      {
        return lhs[i] < rhs[i];
      }
      else
      {
        // strings are the same so far, so continue
        i += 1;
      }
    }
  }

  return lhs.size() < rhs.size();
}

bool
StringUtil::starts_with(const std::string& str, const std::string& prefix)
{
  return str.rfind(prefix, 0) == 0;
}

std::string
StringUtil::tolower(const std::string& text)
{
  std::string result = text;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

std::string
StringUtil::replace_all(const std::string& haystack, const std::string& needle,
                        const std::string& replacement)
{
  std::string ret = haystack;
  size_t start_pos = 0;
  while((start_pos = ret.find(needle, start_pos)) != std::string::npos) {
    ret.replace(start_pos, needle.length(), replacement);
    start_pos += replacement.length();
  }
  return ret;
}

void
StringUtil::split(std::vector<std::string>& output, const std::string& str, char ch)
{
  std::stringstream stream(str);
  std::string element;
  while(getline(stream, element, ch))
    output.push_back(element);
}

/* EOF */
