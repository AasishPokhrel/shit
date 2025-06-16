//  SuperTux
//  Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
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

#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "util/string_util.hpp"

TEST(StringUtilTest, numeric_sort_test)
{
  std::vector<std::string> unsorted_lst =
    {
      "B1235",
      "A123",
      "A123",
      "A12",
      "B12323423A233",
      "B12323423A1231",
      "Z1",
      "A1A123",
      "A1A1",
      "A1A12"
    };

  /* FIXME: this is the result from 'sort -n', which is different from
     what StringUtil::numeric_less produces.

  std::vector<std::string> sorted_lst =
    {
      "A12",
      "A123",
      "A123",
      "A1A1",
      "A1A12"
      "A1A123",
      "B12323423A1231",
      "B12323423A233",
      "B1235",
      "Z1",
    };
  */

  std::vector<std::string> actual_lst =
    {
      "A1A1",
      "A1A12",
      "A1A123",
      "A12",
      "A123",
      "A123",
      "B1235",
      "B12323423A233",
      "B12323423A1231",
      "Z1"
    };

  std::sort(unsorted_lst.begin(), unsorted_lst.end(), StringUtil::numeric_less);

  ASSERT_EQ(actual_lst, unsorted_lst);
}

/* EOF */
