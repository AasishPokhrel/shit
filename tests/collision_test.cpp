//  SuperTux
//  Copyright (C) 2016 Tapesh Mandal <tapesh.mandal@gmail.com>
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

TEST(collisionTest, overlaps_test)
{
    Rectf r1(1.0,4.0,2.0,5.0);
    Rectf r2(6.0,8.0,10.0,9.0);

    ASSERT_EQ(false, r1.overlaps(r2));

    Rectf r3(8.0,3.0,10.0,5.0);
    Rectf r4(2.0,7.0,4.0,9.0);

    ASSERT_EQ(false, r3.overlaps(r4));

    Rectf r5(4.0,1.0,5.0,2.0);
    Rectf r6(8.0,6.0,9.0,10.0);

    ASSERT_EQ(false, r5.overlaps(r6));

    Rectf r7(3.0,8.0,5.0,10.0);
    Rectf r8(7.0,2.0,9.0,4.0);

    ASSERT_EQ(false, r7.overlaps(r8));

    Rectf r9(3.0,6.0,17.0,15.0);
    Rectf r10(9.0,7.0,10.0,8.0);

    ASSERT_EQ(true, r9.overlaps(r10));
}

/* EOF */
