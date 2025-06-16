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

#ifndef WRITER_HEADER
#define WRITER_HEADER

#include <string>
#include <vector>

#include "class.hpp"

namespace Writer
{
  std::string format_description(std::string desc);

  std::string write_file_notice(const std::string& template_file);

  std::string write_inheritance_list(const std::vector<Class>& classes,
                                     const Class::BaseClasses& base_classes,
                                     const std::vector<std::string>& derived_classes);
  std::string write_constants_table(const std::vector<Constant>& constants);
  std::string write_variables_table(const std::vector<Variable>& variables);
  std::string write_function_table(const std::vector<Function>& functions);
  std::string write_class_list(const std::vector<Class>& classes);

  std::string write_class_ref(const std::string& name);
} // namespace Writer

#endif
