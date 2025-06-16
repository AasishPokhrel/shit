//  SuperTux
//  Copyright (C) 2021 Ingo Ruhnke <grumbel@gmail.com>
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

#include <array>
#include <fstream>
#include <iostream>

#include <physfs.h>

#include "physfs/ifile_stream.hpp"

TEST(IFileStreamTest, test)
{
  PHYSFS_init("ifile_stream_test");
  PHYSFS_mount("../tests/data", nullptr, 1);

  IFileStream in("test.dat");

  size_t total_bytes = 0;
  std::array<char, 1024> buffer;
  while (in.read(buffer.data(), buffer.size())) {
    total_bytes += in.gcount();
    ASSERT_EQ(total_bytes, in.tellg());
  };
  total_bytes += in.gcount();

  // tellg() will return -1 instead of the actual value as long as
  // eofbit is set
  in.clear();

  ASSERT_EQ(total_bytes, in.tellg());

  std::ifstream fin("../tests/data/test.dat");
  fin.seekg(0, std::ios::end);
  ASSERT_EQ(fin.tellg(), in.tellg());
}

/* EOF */
