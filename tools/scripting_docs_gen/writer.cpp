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

#include "writer.hpp"

#include <sstream>

#include "util.hpp"

namespace Writer {

std::string format_description(std::string desc)
{
  replace(desc, "\"\"", "`");
  replace(desc, "NOTE:", "<br /><br />**NOTE:**");
  replace(desc, "Note:", "<br /><br />**NOTE:**");
  return desc;
}


std::string write_file_notice(const std::string& template_file)
{
  std::stringstream notice;

  notice << "> This file is auto-generated from the [SuperTux source code](https://github.com/SuperTux/supertux/tree/master/src), "
         << "using the template [" << template_file << "](https://github.com/SuperTux/wiki/tree/master/templates/" << template_file << ")."
         << std::endl << std::endl;

  return notice.str();
}


std::string write_inheritance_list(const std::vector<Class>& classes,
                                   const Class::BaseClasses& base_classes,
                                   const std::vector<std::string>& derived_classes)
{
  std::stringstream list;

  if (!base_classes.empty())
  {
    list << "This class inherits functions and variables from the following base classes:" << std::endl;
    // List of all base classes
    for (const auto& [_, base_class] : base_classes)
    {
      // Check whether the current class is documented
      const bool documented = std::any_of(classes.begin(), classes.end(), [base_class](const Class& cl) { return cl.name == base_class; });

      list << "* " << (documented ? write_class_ref(base_class) : base_class) << std::endl;
    }
    if (!derived_classes.empty())
      list << std::endl;
  }
  if (!derived_classes.empty())
  {
    list << "The following classes inherit functions and variables from this class:" << std::endl;
    // List of all derived classes
    for (const std::string& derived_class : derived_classes)
    {
      // Check whether the current class is documented
      const bool documented = std::any_of(classes.begin(), classes.end(), [derived_class](const Class& cl) { return cl.name == derived_class; });

      list << "* " << (documented ? write_class_ref(derived_class) : derived_class) << std::endl;
    }
  }

  return list.str();
}

std::string write_constants_table(const std::vector<Constant>& constants)
{
  if (constants.empty()) return "";

  std::stringstream table;

  // Table beginning
  table << "Constant | Explanation" << std::endl;
  table << "---------|---------" << std::endl;
  // Table contents (constants)
  for (const Constant& con : constants)
  {
    // Print out type, name, initializer (if available) and description
    table << "`" << con.type << " " << con.name << (con.initializer.empty() ? "" : " " + con.initializer)
          << "` | " << format_description(con.description) << std::endl;
  }

  return table.str();
}

std::string write_variables_table(const std::vector<Variable>& variables)
{
  if (variables.empty()) return "";

  std::stringstream table;

  // Table beginning
  table << "Variable | Explanation" << std::endl;
  table << "---------|---------" << std::endl;
  // Table contents (variables)
  for (const Variable& var : variables)
  {
    // Print out type, name and description
    table << "`" << var.type << " " << var.name << "` | " << format_description(var.description) << std::endl;
  }

  return table.str();
}

std::string write_function_table(const std::vector<Function>& functions)
{
  if (functions.empty()) return "";

  std::stringstream table;

  // Table beginning
  table << "Method | Explanation" << std::endl;
  table << "-------|-------" << std::endl;
  // Table contents (functions)
  for (const Function& func : functions)
  {
    // Print out function
    table << "`" << func.type << " " << func.name << "(";
    for (size_t i = 0; i < func.parameters.size(); i++)
    {
      if (i != 0) table << ", ";
      table << func.parameters[i].type << " " << func.parameters[i].name;
      if (!func.parameters[i].default_value.empty())
        table << " = " << func.parameters[i].default_value;
    }
    table << ")`";

    // Print out description of function
    table << " | ";
    if (func.deprecated)
    {
      table << "**Deprecated!**";
      if (!func.deprecation_msg.empty())
        table << " " << format_description(func.deprecation_msg);

      // Add line breaks only if a description is available
      if (!func.description.empty())
        table << "<br /><br />";
    }
    table << format_description(func.description);

    // Print out descriptions of parameters
    if (std::any_of(func.parameters.begin(), func.parameters.end(), [](const Parameter& param) { return !param.description.empty(); }))
    {
      if (!func.deprecation_msg.empty() || !func.description.empty())
        table << "<br /><br />";

      bool has_printed_param_desc = false;
      for (const Parameter& param : func.parameters)
      {
        if (param.description.empty()) continue;

        table << (has_printed_param_desc ? "<br />" : "") << " `" << param.name << "` - " << format_description(param.description);
        has_printed_param_desc = true;
      }
    }

    // End table entry
    table << std::endl;
  }

  return table.str();
}

std::string write_class_list(const std::vector<Class>& classes)
{
  std::stringstream list;

  // For each class name, create an list entry
  for (const Class& cl : classes)
  {
    list << "* " << write_class_ref(cl.name) << std::endl;
  }

  return list.str();
}


std::string write_class_ref(const std::string& name)
{
  return "[" + name + "](https://github.com/SuperTux/supertux/wiki/Scripting" + name + ")";
}

} // namespace Writer
