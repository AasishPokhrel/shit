//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "math/rectf.hpp"

TEST(RectfTest, contains_point)
{
  ASSERT_TRUE(Rectf(100.0f, 100.0f, 200.0f, 200.0f).contains(Vector(150.0f, 150.0f)));
  ASSERT_FALSE(Rectf(100.0f, 100.0f, 200.0f, 200.0f).contains(Vector(250.0f, 150.0f)));
}

TEST(RectfTest, overlaps_rect)
{
  ASSERT_TRUE(Rectf(100.0f, 100.0f, 200.0f, 200.0f).overlaps(Rectf(150.0f, 150.0f, 190.0f, 190.0f)));
  ASSERT_TRUE(Rectf(100.0f, 100.0f, 200.0f, 200.0f).overlaps(Rectf(100.0f, 100.0f, 200.0f, 200.0f)));
  ASSERT_FALSE(Rectf(100.0f, 100.0f, 200.0f, 200.0f).overlaps(Rectf(250.0f, 250.0f, 300.0f, 300.0f)));
}


TEST(RectfTest, moved)
{
  ASSERT_EQ(Rectf(0.0f, 0.0f, 100.0f, 300.0f).moved(Vector(16.0f, 32.0f)), Rectf(16.0f, 32.0f, 116.0f, 332.0f));
  ASSERT_EQ(Rectf(0.0f, 0.0f, 100.0f, 300.0f).moved(Vector(-16.0f, -32.0f)), Rectf(-16.0f, -32.0f, 84.0f, 268.0f));
}

TEST(RectfTest, set_leftrighttopbottom)
{
  Rectf rect(0.0f, 50.0f, 100.0f, 150.0f);

  rect.set_left(10.0f);
  ASSERT_EQ(Rectf(10.0f, 50.0f, 100.0f, 150.0f), rect);

  rect.set_right(200.0f);
  ASSERT_EQ(Rectf(10.0f, 50.0f, 200.0f, 150.0f), rect);

  rect.set_top(100.0f);
  ASSERT_EQ(Rectf(10.0f, 100.0f, 200.0f, 150.0f), rect);

  rect.set_bottom(200.0f);
  ASSERT_EQ(Rectf(10.0f, 100.0f, 200.0f, 200.0f), rect);
}

TEST(RectfTest, size)
{
  ASSERT_EQ(Rectf(50.0f, 50.0f, 100.0f, 300.0f).get_width(), 50.0f);
  ASSERT_EQ(Rectf(50.0f, 50.0f, 100.0f, 300.0f).get_height(), 250.0f);
}

TEST(RectfTest, from_center)
{
  ASSERT_EQ(Rectf::from_center({16.0f, 16.0f}, {32.0f, 32.0f}), Rectf(0.0f, 0.0f, 32.0f, 32.0f));
}

TEST(RectfTest, set_p1)
{
  Rectf rect(Vector(16.0f, 16.0f), Vector(32.0f, 32.0f));
  rect.set_p1({1.0f, 5.0f});
  ASSERT_EQ(Rectf(Vector(1.0f, 5.0f), Vector(32.0f, 32.0f)), rect);
}

TEST(RectfTest, set_p2)
{
  Rectf rect(Vector(16.0f, 16.0f), Vector(32.0f, 32.0f));
  rect.set_p2({48.0f, 100.0f});
  ASSERT_EQ(Rectf(Vector(16.0f, 16.0f), Vector(48.0f, 100.0f)), rect);
}

/* EOF */
