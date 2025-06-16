//  SuperTux - Scripting reference generator
//  Copyright (C) 2023 Vankata453
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

#ifndef CLASS_HEADER
#define CLASS_HEADER

#include <map>
#include <string>
#include <vector>

struct Constant
{
  std::string type {};
  std::string name {};
  std::string initializer {};
  std::string description {};
};

struct Variable
{
  std::string type {};
  std::string name {};
  std::string description {};
};

struct Parameter
{
  std::string type {};
  std::string name {};
  std::string description {};
  std::string default_value {};
};

struct Function
{
  std::string type {};
  std::string name {};
  std::string description {};
  std::vector<Parameter> parameters {};

  bool deprecated {};
  std::string deprecation_msg {};
};

struct Class
{
  std::string name {};
  std::string summary {};
  std::string instances {};
  std::vector<Constant> constants {};
  std::vector<Variable> variables {};
  std::vector<Function> functions {};

  typedef std::map<int, std::string> BaseClasses;
  BaseClasses base_classes {};
  std::vector<std::string> derived_classes {};
};

#endif
