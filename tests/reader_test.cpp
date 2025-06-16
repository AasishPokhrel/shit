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

#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"

TEST(ReaderTest, get)
{
  std::istringstream in(
    "(supertux-test\n"
    "   (mybool #t)\r"
    "   (myint 123456789)\r\n"
    "   (myfloat 1.125)\n\r"
    "   (mystring \"Hello World\")\n"
    "   (mystringtrans (_ \"Hello World\"))\n"
    "   (myboolarray #t #f #t #f)\n"
    "   (myintarray 5 5 4 4 3 2 1 0)\n"
    "   (myfloatarray 6.5 5.25 4.125 3.0625 2.0 1.0 0.5 0.25 0.125)\n"
    "   (mystringarray \"One\" \"Two\" \"Three\")\n"
    "   (mymapping (a 1) (b 2))\n"
    "   (mycustom \"1234\")\n"
    ")\n");

  auto doc = ReaderDocument::from_stream(in);
  auto root = doc.get_root();
  ASSERT_EQ("supertux-test", root.get_name());
  auto mapping = root.get_mapping();

  {
    bool mybool;
    mapping.get("mybool", mybool);
    ASSERT_EQ(true, mybool);
  }

  {
    int myint;
    mapping.get("myint", myint);
    ASSERT_EQ(123456789, myint);
  }

  {
    float myfloat;
    mapping.get("myfloat", myfloat);
    ASSERT_EQ(1.125, myfloat);
  }

  {
    std::string mystring;
    mapping.get("mystring", mystring);
    ASSERT_EQ("Hello World", mystring);
  }

  {
    std::string mystringtrans;
    mapping.get("mystringtrans", mystringtrans);
    ASSERT_EQ("Hello World", mystringtrans);
  }

  {
    std::vector<bool> expected{ true, false, true, false };
    std::vector<bool> result;
    mapping.get("myboolarray", result);
    ASSERT_EQ(expected, result);
  }

  {
    std::vector<int> expected{ 5, 5, 4, 4, 3, 2, 1, 0 };
    std::vector<int> result;
    mapping.get("myintarray", result);
    ASSERT_EQ(expected, result);
  }

  {
    std::vector<float> expected({6.5f, 5.25f, 4.125f, 3.0625f, 2.0f, 1.0f, 0.5f, 0.25f, 0.125f});
    std::vector<float> result;
    mapping.get("myfloatarray", result);
    ASSERT_EQ(expected, result);
  }

  {
    std::vector<std::string> expected{"One", "Two", "Three"};
    std::vector<std::string> result;
    mapping.get("mystringarray", result);
    ASSERT_EQ(expected, result);
  }

  {
    std::optional<ReaderMapping> child_mapping;
    mapping.get("mymapping", child_mapping);

    int a;
    child_mapping->get("a", a);
    ASSERT_EQ(1, a);

    int b;
    child_mapping->get("b", b);
    ASSERT_EQ(2, b);
  }

  {
    auto from_string = [](const std::string& text){ return std::stoi(text); };

    int value = 0;
    mapping.get_custom("mycustom", value, from_string);
    ASSERT_EQ(1234, value);

    int value2 = 0;
    mapping.get_custom("does-not-exist", value2, from_string);
    ASSERT_EQ(0, value2);

    int value3 = 0;
    mapping.get_custom("does-not-exist", value3, from_string, 4321);
    ASSERT_EQ(4321, value3);
  }

  {
    bool mybool;
    int myint;
    float myfloat;
    ASSERT_THROW({mapping.get("mybool", myfloat);}, std::runtime_error);
    ASSERT_THROW({mapping.get("myint", mybool);}, std::runtime_error);
    ASSERT_THROW({mapping.get("myfloat", myint);}, std::runtime_error);
    ASSERT_THROW({mapping.get("mymapping", myint);}, std::runtime_error);
  }
}

TEST(ReaderTest, get_compressed)
{
  std::istringstream in(
    "(supertux-test\n"
    "   (mycompressedintarray -6 0 45 -4 1 -5 3 -2 0)\n"
    ")\n");

  auto doc = ReaderDocument::from_stream(in);
  auto root = doc.get_root();
  ASSERT_EQ("supertux-test", root.get_name());
  auto mapping = root.get_mapping();

  {
    std::vector<unsigned int> expected{ 0, 0, 0, 0, 0, 0, 45, 1, 1, 1, 1, 3, 3, 3, 3, 3, 0, 0, };
    std::vector<unsigned int> result;
    mapping.get_compressed("mycompressedintarray", result);
    ASSERT_EQ(expected, result);
  }
}

TEST(ReaderTest, syntax_error)
{
  std::istringstream in(
    "(supertux-test\n"
    "   (mybool #t err)\r"
    "   (myint 123456789 err)\r\n"
    "   (myfloat 1.125 err)\n\r"
    "   (mystring \"Hello World\" err)\n"
    "   (mystringtrans (_ \"Hello World\" err))\n"
    "   (mycompressedintarray1 -5 0 43 -5 -12 3 -1 5)\n"
    "   (mycompressedintarray2 -5 0 43 -5 12 -3 1 -5)\n"
    "   (mymapping err (a 1) (b 2))\n"
    ")\n");

  auto doc = ReaderDocument::from_stream(in);
  auto root = doc.get_root();
  ASSERT_EQ("supertux-test", root.get_name());
  auto mapping = root.get_mapping();

  bool mybool;
  int myint;
  float myfloat;
  std::vector<unsigned int> mycompressedintarray;
  ASSERT_THROW({mapping.get("mybool", mybool);}, std::runtime_error);
  ASSERT_THROW({mapping.get("myint", myint);}, std::runtime_error);
  ASSERT_THROW({mapping.get("myfloat", myfloat);}, std::runtime_error);
  ASSERT_THROW({mapping.get_compressed("mycompressedintarray1", mycompressedintarray);}, std::runtime_error);
  ASSERT_THROW({mapping.get_compressed("mycompressedintarray2", mycompressedintarray);}, std::runtime_error);

  std::optional<ReaderMapping> mymapping;
  mapping.get("mymapping", mymapping);
  ASSERT_THROW({mymapping->get("a", myint);}, std::runtime_error);
  ASSERT_THROW({mymapping->get("b", myint);}, std::runtime_error);
}

/* EOF */
