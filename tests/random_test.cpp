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

#include "math/random.hpp"

TEST(RandomTest, rand1i)
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    int v = random.rand(10);
    ASSERT_LE(0, v);
    ASSERT_LT(v, 10);
  }
}

TEST(RandomTest, rand2i)
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    int v = random.rand(10, 20);
    ASSERT_LE(10, v);
    ASSERT_LT(v, 20);
  }
}

TEST(RandomTest, rand1f)
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    float v = random.randf(10);
    ASSERT_LE(0.0f, v);
    ASSERT_LT(v, 10.0f);
  }
}

TEST(RandomTest, rand2f)
{
  Random random;
  random.seed(0);
  for(int i = 0; i < 1000; ++i)
  {
    float v = random.randf(10.0f, 20.0f);
    ASSERT_LE(10.0f, v);
    ASSERT_LT(v, 20.0f);
  }
}

TEST(RandomTest, rand_determinism)
{
  Random random;

  random.seed(0);
  std::vector<int> run1;
  run1.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run1.push_back(random.rand());
  }

  random.seed(0);
  std::vector<int> run2;
  run2.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run2.push_back(random.rand());
  }

  ASSERT_EQ(run1, run2);
}

TEST(RandomTest, randf_determinism)
{
  Random random;

  random.seed(0);
  std::vector<float> run1;
  run1.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run1.push_back(random.randf(1.0f));
  }

  random.seed(0);
  std::vector<float> run2;
  run2.reserve(1000);
  for(int i = 0; i < 1000; ++i)
  {
    run2.push_back(random.randf(1.0f));
  }

  ASSERT_EQ(run1, run2);
}

/* EOF */
