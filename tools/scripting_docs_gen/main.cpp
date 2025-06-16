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

/** This program converts Doxygen XML output files into 
    Markdown scripting reference documentation, using templates. **/

/* Requirements: C++17, TinyXML2 library */

#include <algorithm>
#include <vector>
#include <iostream>
#include <filesystem>

#include <tinyxml2.h>

#include "class.hpp"
#include "parser.hpp"
#include "util.hpp"
#include "writer.hpp"

int main(int argc, char** argv)
{
  /** Get required parameters **/
  std::string input_dir, home_tmpl_file, page_tmpl_file, output_dir(".");
  for (int i = 1; i < argc; i++)
  {
    if (param_matches(argc, argv, i, "-d", "--dir", "--directory")) // Input directory has been provided
      input_dir = argv[i + 1];
    else if (param_matches(argc, argv, i, "-h", "--home", "--home-template")) // Home template has been provided
      home_tmpl_file = argv[i + 1];
    else if (param_matches(argc, argv, i, "-p", "--page", "--page-template")) // Page template has been provided
      page_tmpl_file = argv[i + 1];
    else if (param_matches(argc, argv, i, "-o", "--output", "--output-directory")) // Output directory has been provided
      output_dir = argv[i + 1];
  }
  if (input_dir.empty() || home_tmpl_file.empty() || page_tmpl_file.empty()) // Do not allow empty parameters
  {
    std::cout << "Usage: [\"-d\", \"--dir\", \"--directory\"] (string [REQUIRED])" << std::endl;
    std::cout << "       [\"-h\", \"--home\", \"--home-template\"] (string [REQUIRED])" << std::endl;
    std::cout << "       [\"-p\", \"--page\", \"--page-template\"] (string [REQUIRED])" << std::endl;
    std::cout << "       [\"-o\", \"--output\", \"--output-directory\"] (string [DEFAULT = \".\" (current)])" << std::endl;
    return 1;
  }

  /** Read template files **/
  const std::string home_template = read_file(home_tmpl_file);
  const std::string page_template = read_file(page_tmpl_file);
  const std::string home_template_filename = std::filesystem::path(home_tmpl_file).filename();
  const std::string page_template_filename = std::filesystem::path(page_tmpl_file).filename();

  /** Prepare other variables **/
  std::vector<Class> classes;
  std::filesystem::path output_dir_path = output_dir;

  /** Loop through all files in the provided directory and parse ones containing XML class data **/
  for (const auto& dir_entry : std::filesystem::recursive_directory_iterator(input_dir))
  {
    std::filesystem::path fspath = dir_entry;
    const std::string filename = fspath.filename();

    if (!(std::filesystem::is_regular_file(dir_entry) &&
          (starts_with(filename, "class") || starts_with(filename, "namespace")))) continue; // Make sure the current file is about a class or a namespace

    /** Read data from current XML class data file **/
    tinyxml2::XMLDocument doc;
    doc.LoadFile(fspath.c_str());

    /** Parse the class and its members **/
    Class cl; // Store class data
    Parser::parse_compounddef(doc.RootElement(), cl);

    if (cl.constants.empty() && cl.variables.empty() && cl.functions.empty()) continue; // If the class has no content, do not save it

    // Save the class
    classes.push_back(std::move(cl));
  }

  // Sort classes by their names (A-Z)
  std::sort(classes.begin(), classes.end(),
            [](const Class& lhs, const Class& rhs) { return lhs.name < rhs.name; });

  /** Loop through all classes and write their data to files **/
  for (Class& cl : classes)
  {
    // Remove a few internal base classes
    for (auto it = cl.base_classes.begin(); it != cl.base_classes.end();)
    {
      if (it->second == "ssq::ExposableClass" || it->second == "ExposableClass")
        cl.base_classes.erase(it++);
      else
        ++it;
    }

    /** Fill in the data in the provided page template file and save as new file.
        File entries to be replaced:
          "${SRG_CLASSSUMMARY}": Insert the provided summary info of the class, if available.
          "${SRG_CLASSINSTANCES}": Insert the provided instances info of the class, if available.
          "${SRG_CLASSCONSTANTS}": Insert a table with all constants in the class and their data.
          "${SRG_FUNCDATATABLE}": Insert a table with all functions in the class and their data.
          "${SRG_CLASSNAME}": Insert the class name.
          "${SRG_REF_[class]}": Insert the name of the referenced class, as well as its reference URL.

        File formatting replacements:
          "${SRG_NEWPARAGRAPH}": Insert two new lines to start a new paragraph.
          "${SRG_TABLENEWPARAGRAPH}": Insert two HTML line breaks to start a new paragraph in a table.
          [""] (double quotation marks): Replace with [`] for code formatting.
    **/
    // Prepare target data (add a notice at the top of the generated file)
    std::string target_data = Writer::write_file_notice(page_template_filename) +  page_template;

    // Entries
    replace(target_data, "${SRG_CLASSSUMMARY}", Writer::format_description(std::move(cl.summary)), "None.");
    replace(target_data, "${SRG_CLASSINSTANCES}", Writer::format_description(std::move(cl.instances)), "None.");
    replace(target_data, "${SRG_CLASSINHERITANCE}", Writer::write_inheritance_list(classes, cl.base_classes, cl.derived_classes), "None.");
    replace(target_data, "${SRG_CLASSCONSTANTS}", Writer::write_constants_table(cl.constants), "None.");
    replace(target_data, "${SRG_CLASSVARIABLES}", Writer::write_variables_table(cl.variables), "None.");
    replace(target_data, "${SRG_CLASSFUNCTIONS}", Writer::write_function_table(cl.functions), "None.");
    replace(target_data, "${SRG_CLASSNAME}", "`" + cl.name + "`");
    regex_replace(target_data, std::regex("\\$\\{SRG_REF_(.+?)\\}"), Writer::write_class_ref("$1"));
    // Formatting
    replace(target_data, "${SRG_NEWPARAGRAPH} ", "\r\n\r\n");
    replace(target_data, "${SRG_TABLENEWPARAGRAPH}", "<br /><br />");

    // Write to target file
    write_file(output_dir_path / std::filesystem::path("Scripting" + cl.name + ".md"), target_data);
    std::cout << "Generated reference for class \"" << cl.name << "\"." << std::endl;
  }

  /** Fill in the data in the provided home page template file and save as new file.
      File entries to be replaced:
        "${SRG_CLASSLIST}": Insert a list with all classes which have been parsed.
  **/
  // Prepare target data (add a notice at the top of the generated file)
  std::string target_data = Writer::write_file_notice(home_template_filename) + home_template;

  // Entries
  replace(target_data, "${SRG_CLASSLIST}", Writer::write_class_list(classes));

  // Write to target file
  write_file(output_dir_path / std::filesystem::path("Scripting_Reference.md"), target_data);

  return 0;
}
