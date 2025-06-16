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

#include "math/sizef.hpp"

TEST(SizefTest, sizef_test)
{
  Sizef size(800, 600);

  ASSERT_EQ(Sizef(800, 600), size);
  ASSERT_EQ(Sizef(1600, 1200), size * 2);
  ASSERT_EQ(Sizef(400, 300), size / 2);
  ASSERT_EQ(Sizef(1000, 900), size + Sizef(200, 300));

  size *= 2;
  ASSERT_EQ(Sizef(1600, 1200), size);

  size /= 2;
  ASSERT_EQ(Sizef(800, 600), size);

  size += size;
  ASSERT_EQ(Sizef(1600, 1200), size);
}

/* EOF */
