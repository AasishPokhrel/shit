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

#include <unordered_set>

#include "util/uid_generator.hpp"

TEST(UIDTest, null)
{
  UIDGenerator generator;
  UID null_uid;
  UID uid = generator.next();

  ASSERT_FALSE(null_uid);
  ASSERT_TRUE(uid);
  ASSERT_TRUE(uid != null_uid);
}

TEST(UIDTest, magic)
{
  UIDGenerator generator1;
  UIDGenerator generator2;

  UID uid1 = generator1.next();
  UID uid2 = generator2.next();

  ASSERT_TRUE(uid1 != uid2);
}

TEST(UIDTest, copy)
{
  UIDGenerator generator;
  UID uid = generator.next();
  UID other = uid;

  ASSERT_EQ(uid, other);
}

TEST(UIDTest, unique)
{
  if ((false)) {
    UIDGenerator generator;
    std::unordered_set<UID> uids;

    for(int i = 0; i < 0xffffff; ++i)
    {
      UID uid = generator.next();
      ASSERT_TRUE(uids.find(uid) == uids.end());
      uids.insert(uid);
    }
  }
}

/* EOF */
