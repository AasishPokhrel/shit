//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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
#include <iostream>
#include <errno.h>
#include <string.h>
#include <vector>
#include <memory>

#include "editor/object_option.hpp"
#include "supertux/game_object.hpp"
#include "video/color.hpp"

TEST(ObjectOption, to_string)
{
  {
    std::string mystring = "field";
    StringObjectOption textfield("test", &mystring, {}, std::nullopt, 0);
    ASSERT_EQ(mystring, textfield.to_string());
  }

  {
    bool mybool = true;
    BoolObjectOption field("test", &mybool, {}, {}, 0);
    ASSERT_EQ("true", field.to_string());
  }

  {
    int myint = 73258;
    IntObjectOption intfield("test", &myint, {}, {}, 0);
    ASSERT_EQ("73258", intfield.to_string());
  }

  {
    float myfloat = 2.125;
    FloatObjectOption numfield("test", &myfloat, {}, {}, 0);
    ASSERT_EQ("2.125", numfield.to_string());
  }

  {
    std::vector<std::string> select = {"foo", "bar"};
    enum FooBar {
      FOO, BAR
    };
    FooBar fb1 = FOO;
    FooBar fb2 = BAR;
    StringSelectObjectOption stringselect1("test", reinterpret_cast<int*>(&fb1), select, {}, {}, 0);
    StringSelectObjectOption stringselect2("test", reinterpret_cast<int*>(&fb2), select, {}, {}, 0);
    ASSERT_EQ("foo", stringselect1.to_string());
    ASSERT_EQ("bar", stringselect2.to_string());
  }

  {
    class TestObject : public GameObject
    {
      public:
        TestObject() : GameObject() {}

        void draw(DrawingContext&) override {}
        void update(float) override {}
    };

    std::vector<std::unique_ptr<GameObject>> select;
    select.push_back(std::make_unique<TestObject>());
    select.push_back(std::make_unique<TestObject>());
    select.push_back(std::make_unique<TestObject>());
    ObjectSelectObjectOption objectselect("test", &select, 0, {}, {}, 0);
    ASSERT_EQ("3", objectselect.to_string());
  }

  {
    Color mycolor = Color::YELLOW;
    ColorObjectOption color("test", &mycolor, {}, {}, false, 0);
    ASSERT_EQ("1.000000 1.000000 0.000000", color.to_string());
  }

  {
    std::string myscript = "sector.set_trolling_mode(true)";
    ScriptObjectOption script("text", &myscript, {}, 0);
    ASSERT_EQ("...", script.to_string());
  }
}

/* EOF */
