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

#include "util/colorspace_oklab.hpp"

#include <gtest/gtest.h>

#include "video/color.hpp"

TEST(ColorOKLCh, ctor)
{
  ColorOKLCh color(.3f, .4f, .5f);

  // Can't use strict equality because of imprecision
  // 1 / 256 = 0.00390625, so 0.001 should make no difference.
  EXPECT_NEAR(color.L, .3f, 0.001f);
  EXPECT_NEAR(color.C, .4f, 0.001f);
  EXPECT_NEAR(color.h, .5f, 0.001f);
}

TEST(ColorOKLCh, ctor_Color)
{
  Color col(.3f, .4f, .5f);
  ColorOKLCh color(col);
  EXPECT_NEAR(color.L, .50023675f, 0.001f);
  EXPECT_NEAR(color.C, .050982524f, 0.001f);
  EXPECT_NEAR(color.h, -1.9393998f, 0.001f);
}

/* EOF */
