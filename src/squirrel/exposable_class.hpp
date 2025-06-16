//  SuperTux
//  Copyright (C) 2024 Vankata453
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

#ifndef HEADER_SUPERTUX_SQUIRREL_EXPOSABLE_CLASS_HPP
#define HEADER_SUPERTUX_SQUIRREL_EXPOSABLE_CLASS_HPP

#include <simplesquirrel/exposable_class.hpp>

#include <string>

/** Represents a class, which can be exposed to scripting. */
class ExposableClass : public ssq::ExposableClass
{
public:
  ExposableClass() {}
  virtual ~ExposableClass() override {}

  virtual std::string get_exposed_class_name() const = 0;

private:
  ExposableClass(const ExposableClass&) = delete;
  ExposableClass& operator=(const ExposableClass&) = delete;
};

#endif

/* EOF */
