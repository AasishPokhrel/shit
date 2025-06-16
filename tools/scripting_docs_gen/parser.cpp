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

#include "parser.hpp"

#include <string>
#include <iostream>
#include <unordered_map>

#include "util.hpp"

namespace Parser {

// Simplified versions of various C++ types for easier understanding
static const std::unordered_map<std::string, std::string> s_simplified_types = {
  { "SQInteger", "ANY" }, // SQInteger is used internally with Squirrel, so we don't know the exact return type
  { "char", "int" },
  { "signed char", "int" },
  { "short", "int" },
  { "long", "int" },
  { "unsigned char", "int" },
  { "unsigned short", "int" },
  { "unsigned int", "int" },
  { "unsigned long", "int" },
  { "uint8_t", "int" },
  { "uint16_t", "int" },
  { "uint32_t", "int" },
  { "long long", "int" },
  { "unsigned long long", "int" },
  { "double", "float" },
  { "std::string", "string" },
  { "std::wstring", "string" },
  { "const std::string &", "string" },
  { "const std::wstring &", "string" }
};

static void parse_base_classes(tinyxml2::XMLElement* p_inheritancenode, tinyxml2::XMLElement* p_inheritancegraph, Class::BaseClasses& list)
{
  list[std::stoi(p_inheritancenode->FindAttribute("id")->Value())] = p_inheritancenode->FirstChildElement("label")->GetText();

  tinyxml2::XMLElement* p_childnode = p_inheritancenode->FirstChildElement("childnode");
  while (p_childnode)
  {
    tinyxml2::XMLElement* p_childinheritancenode = first_child_with_attribute(p_inheritancegraph, "node", "id", p_childnode->FindAttribute("refid")->Value());
    if (p_childinheritancenode)
      parse_base_classes(p_childinheritancenode, p_inheritancegraph, list);

    p_childnode = p_childnode->NextSiblingElement("childnode");
  }
}


void parse_compounddef(tinyxml2::XMLElement* p_root, Class& cls)
{
  tinyxml2::XMLElement* p_compounddef = p_root->FirstChildElement("compounddef");

  // Get general class info
  const char* name = p_compounddef->FirstChildElement("compoundname")->GetText();
  if (!name) return; // Some namespaces may not have a name. Don't include those.
  cls.name = name;

  // Leave only the class name
  const size_t pos = cls.name.find_last_of("::");
  if (pos != std::string::npos)
    cls.name.erase(0, pos + 1);

  // Get additional info
  tinyxml2::XMLElement* p_detaileddescpara = p_compounddef->FirstChildElement("detaileddescription")->FirstChildElement("para");
  if (p_detaileddescpara) // Detailed description (possibly containing additional info) is available
  {
    // Check whether the class is marked with "@scripting". Otherwise, do not include.
    bool include = false;
    tinyxml2::XMLElement* p_xrefsect = p_detaileddescpara->FirstChildElement("xrefsect");
    while (p_xrefsect)
    {
      if (el_equal(p_xrefsect, "xreftitle", "Scripting"))
      {
        include = true;
        break;
      }
      p_xrefsect = p_xrefsect->NextSiblingElement("xrefsect");
    }
    if (!include) return;

    p_xrefsect = p_detaileddescpara->FirstChildElement("xrefsect");
    while (p_xrefsect)
    {
      // Check if the "xrefsect" title matches any of the additional info titles
      const std::string title = p_xrefsect->FirstChildElement("xreftitle")->GetText();
      if (title == "Summary")
        parse_xrefsect_desc(p_xrefsect, cls.summary);
      else if (title == "Instances")
        parse_xrefsect_desc(p_xrefsect, cls.instances);

      p_xrefsect = p_xrefsect->NextSiblingElement("xrefsect");
    }
  }

  parse_sectiondef(p_compounddef, cls);

  tinyxml2::XMLElement* p_inheritancegraph = p_compounddef->FirstChildElement("inheritancegraph");
  if (p_inheritancegraph)
  {
    // Get base classes
    tinyxml2::XMLElement* p_inheritancenode = first_child_with_attribute(p_inheritancegraph, "node", "id", "1");
    if (p_inheritancenode)
      parse_base_classes(p_inheritancenode, p_inheritancegraph, cls.base_classes);

    cls.base_classes.erase(1); // The node with an ID of "1" is this class

    // Get derived (child) classes
    p_inheritancenode = p_inheritancegraph->FirstChildElement("node");
    while (p_inheritancenode)
    {
      const int id = std::stoi(p_inheritancenode->FindAttribute("id")->Value());
      if (id != 1 && cls.base_classes.find(id) == cls.base_classes.end()) // Make sure this is not this class, or a base class
      {
        std::string name = p_inheritancenode->FirstChildElement("label")->GetText();

        // Leave only the class name
        const size_t pos = name.find_last_of("::");
        if (pos != std::string::npos)
          name.erase(0, pos + 1);

        cls.derived_classes.push_back(name);
      }

      p_inheritancenode = p_inheritancenode->NextSiblingElement("node");
    }
    std::sort(cls.derived_classes.begin(), cls.derived_classes.end()); // Sort A-Z
    cls.derived_classes.erase(std::unique(cls.derived_classes.begin(), cls.derived_classes.end()), cls.derived_classes.end()); // Remove duplicates
  }
}

void parse_sectiondef(tinyxml2::XMLElement* p_compounddef, Class& cls)
{
  tinyxml2::XMLElement* p_sectiondef = p_compounddef->FirstChildElement("sectiondef");
  while (p_sectiondef)
  {
    parse_memberdef(p_sectiondef, cls);

    p_sectiondef = p_sectiondef->NextSiblingElement("sectiondef");
  }
}

void parse_memberdef(tinyxml2::XMLElement* p_sectiondef, Class& cls)
{
  tinyxml2::XMLElement* p_memberdef = p_sectiondef->FirstChildElement("memberdef");
  while (p_memberdef)
  {
    if (attr_equal(p_memberdef, "kind", "function") &&
        //!el_equal(p_memberdef, "type", "") &&
        !p_memberdef->FirstChildElement("reimplements")) // Look for non-derived typed functions
    {
      parse_function(p_memberdef, cls);
    }
    else if (attr_equal(p_memberdef, "kind", "enum")) // Look for enumerators
    {
      tinyxml2::XMLElement* p_detaileddescpara = p_memberdef->FirstChildElement("detaileddescription")->FirstChildElement("para");
      if (p_detaileddescpara)
      {
        bool include = false;
        std::string prefix;

        tinyxml2::XMLElement* p_xrefsect = p_detaileddescpara->FirstChildElement("xrefsect");
        while (p_xrefsect)
        {
          if (el_equal(p_xrefsect, "xreftitle", "Scripting")) // Make sure the enum is marked with "@scripting". Otherwise, do not include.
            include = true;
          else if (el_equal(p_xrefsect, "xreftitle", "Prefix"))
            parse_xrefsect_desc(p_xrefsect, prefix);

          p_xrefsect = p_xrefsect->NextSiblingElement("xrefsect");
        }
        if (include)
        {
          tinyxml2::XMLElement* p_enumvalue = p_memberdef->FirstChildElement("enumvalue");
          while (p_enumvalue)
          {
            parse_constant(p_enumvalue, cls, true, prefix); // Parse enumerators as constants

            p_enumvalue = p_enumvalue->NextSiblingElement("enumvalue");
          }
        }
      }
    }
    else if (attr_equal(p_memberdef, "kind", "variable") &&
             !el_equal(p_memberdef, "type", "")) // Look for variables
    {
      if (starts_with(p_memberdef->FirstChildElement("type")->GetText(), "const ")) // Constant variable
        parse_constant(p_memberdef, cls);
      else // Non-constant variable
        parse_variable(p_memberdef, cls);
    }
    p_memberdef = p_memberdef->NextSiblingElement("memberdef");
  }
}


void parse_constant(tinyxml2::XMLElement* p_memberdef, Class& cls, bool include, const std::string& prefix)
{
  /** Parse the constant **/
  Constant con;

  // Get general info
  tinyxml2::XMLElement* p_type = p_memberdef->FirstChildElement("type");
  if (p_type)
  {
    con.type = p_type->GetText();
    con.type.erase(0, 6); // Remove the "const " part from the type string
  }
  else // Would indicate this is an enumerator
  {
    con.type = "int";
  }
  con.name = p_memberdef->FirstChildElement("name")->GetText();
  if (starts_with(con.name, "s_"))
    con.name.erase(0, 2); // Remove the "s_" prefix
  con.name = prefix + con.name;
  tinyxml2::XMLElement* p_initializer = p_memberdef->FirstChildElement("initializer");
  if (p_initializer)
    con.initializer = p_initializer->GetText();
  tinyxml2::XMLElement* p_descpara = p_memberdef->FirstChildElement("briefdescription")->FirstChildElement("para");
  tinyxml2::XMLElement* p_detaileddescpara = p_memberdef->FirstChildElement("detaileddescription")->FirstChildElement("para");

  if (p_detaileddescpara)
  {
    tinyxml2::XMLElement* p_xrefsect = p_detaileddescpara->FirstChildElement("xrefsect");
    while (p_xrefsect)
    {
      if (el_equal(p_xrefsect, "xreftitle", "Scripting")) // Make sure the constant is marked with "@scripting". Otherwise, do not include.
        include = true;
      else if (el_equal(p_xrefsect, "xreftitle", "Description"))
        parse_xrefsect_desc(p_xrefsect, con.description);

      p_xrefsect = p_xrefsect->NextSiblingElement("xrefsect");
    }
    if (!include) return;

    if (p_descpara) // Brief description has been provided
    {
      XMLTextReader read(con.description);
      p_descpara->Accept(&read);
    }

    // Add to constants list
    cls.constants.push_back(std::move(con));
  }
}

void parse_variable(tinyxml2::XMLElement* p_memberdef, Class& cls)
{
  /** Parse the variable **/
  Variable var;

  // Get general info
  var.type = p_memberdef->FirstChildElement("type")->GetText();
  var.name = p_memberdef->FirstChildElement("name")->GetText();
  if (starts_with(var.name, "m_"))
    var.name.erase(0, 2); // Remove the "m_" prefix
  tinyxml2::XMLElement* p_descpara = p_memberdef->FirstChildElement("briefdescription")->FirstChildElement("para");
  tinyxml2::XMLElement* p_detaileddescpara = p_memberdef->FirstChildElement("detaileddescription")->FirstChildElement("para");

  if (p_detaileddescpara)
  {
    bool include = false;
    tinyxml2::XMLElement* p_xrefsect = p_detaileddescpara->FirstChildElement("xrefsect");
    while (p_xrefsect)
    {
      if (el_equal(p_xrefsect, "xreftitle", "Scripting")) // Make sure the variable is marked with "@scripting". Otherwise, do not include.
        include = true;
      else if (el_equal(p_xrefsect, "xreftitle", "Description"))
        parse_xrefsect_desc(p_xrefsect, var.description);

      p_xrefsect = p_xrefsect->NextSiblingElement("xrefsect");
    }
    if (!include) return;

    if (p_descpara) // Brief description has been provided
    {
      XMLTextReader read(var.description);
      p_descpara->Accept(&read);
    }

    // Add to variables list
    cls.variables.push_back(std::move(var));
  }
}

void parse_function(tinyxml2::XMLElement* p_memberdef, Class& cls)
{
  /** Parse the function **/
  Function func;

  // Get general info
  const char* type = p_memberdef->FirstChildElement("type")->GetText();
  if (type)
  {
    func.type = type;

    // Replace type with simplified version, if available
    const auto it = s_simplified_types.find(func.type);
    if (it != s_simplified_types.end())
      func.type = it->second;
  }
  else
  {
    func.type = "void";
  }
  func.name = p_memberdef->FirstChildElement("name")->GetText();

  tinyxml2::XMLElement* p_descpara = p_memberdef->FirstChildElement("briefdescription")->FirstChildElement("para");
  tinyxml2::XMLElement* p_detaileddescpara = p_memberdef->FirstChildElement("detaileddescription")->FirstChildElement("para");

  if (p_detaileddescpara)
  {
    bool include = false;
    tinyxml2::XMLElement* p_xrefsect = p_detaileddescpara->FirstChildElement("xrefsect");
    while (p_xrefsect)
    {
      if (el_equal(p_xrefsect, "xreftitle", "Scripting")) // Make sure the function is marked with "@scripting". Otherwise, do not include.
        include = true;
      else if (el_equal(p_xrefsect, "xreftitle", "Deprecated"))
      {
        func.deprecated = true;
        parse_xrefsect_desc(p_xrefsect, func.deprecation_msg);
      }
      else if (el_equal(p_xrefsect, "xreftitle", "Description"))
        parse_xrefsect_desc(p_xrefsect, func.description);

      p_xrefsect = p_xrefsect->NextSiblingElement("xrefsect");
    }
    if (include)
    {
      tinyxml2::XMLElement* p_simplesect = p_detaileddescpara->FirstChildElement("simplesect");
      while (p_simplesect)
      {
        if (attr_equal(p_simplesect, "kind", "return")) // Custom return type specified
        {
          func.type = p_simplesect->FirstChildElement("para")->GetText();
          func.type.pop_back(); // Remove space at the end
          break;
        }
        p_simplesect = p_simplesect->NextSiblingElement("simplesect");
      }

      if (p_descpara) // Brief description has been provided
      {
        XMLTextReader read(func.description);
        p_descpara->Accept(&read);
      }

      /** Parse function parameters **/
      parse_parameterlist(p_memberdef, func);

      // Add to function list
      cls.functions.push_back(std::move(func));
    }
  }
}

void parse_parameterlist(tinyxml2::XMLElement* p_memberdef, Function& func)
{
  tinyxml2::XMLElement* p_detaileddescpara = p_memberdef->FirstChildElement("detaileddescription")->FirstChildElement("para");
  if (!p_detaileddescpara) return; // There is no detailed description, hence no parameter list

  tinyxml2::XMLElement* p_parameterlist = p_detaileddescpara->FirstChildElement("parameterlist");
  if (!p_parameterlist) return; // No parameter list

  tinyxml2::XMLElement* p_parameteritem = p_parameterlist->FirstChildElement("parameteritem");
  while (p_parameteritem)
  {
    /** Parse the parameter **/
    Parameter param;

    tinyxml2::XMLElement* p_parameternamelist = p_parameteritem->FirstChildElement("parameternamelist");
    tinyxml2::XMLElement* p_parametertype = p_parameternamelist->FirstChildElement("parametertype");
    if (p_parametertype) // Type has been provided
      param.type = p_parametertype->GetText();

    tinyxml2::XMLElement* p_parametername = p_parameternamelist->FirstChildElement("parametername");
    if (p_parametername) // Name has been provided
      param.name = p_parametername->GetText();
    if (starts_with(param.name, "$")) // Parameter names may start with "$" so they can have types assigned. Remove the "$" if so
      param.name.erase(0, 1);

    tinyxml2::XMLElement* p_parameterdescpara = p_parameteritem->FirstChildElement("parameterdescription")->FirstChildElement("para");
    if (p_parameterdescpara) // Description has been provided
    {
      XMLTextReader read(param.description);
      p_parameterdescpara->Accept(&read);
    }

    // Add to parameter list
    func.parameters.push_back(std::move(param));

    p_parameteritem = p_parameteritem->NextSiblingElement("parameteritem");
  }

  // Get default parameter values
  tinyxml2::XMLElement* p_param = p_memberdef->FirstChildElement("param");
  while (p_param)
  {
    tinyxml2::XMLElement* p_defval = p_param->FirstChildElement("defval");
    if (p_defval)
    {
      tinyxml2::XMLElement* p_declname = p_param->FirstChildElement("declname");
      for (Parameter& param : func.parameters)
      {
        if (!strcmp(p_declname->GetText(), param.name.c_str()))
        {
          param.default_value = p_defval->GetText();
          break;
        }
      }
    }

    p_param = p_param->NextSiblingElement("param");
  }
}


void parse_xrefsect_desc(tinyxml2::XMLElement* p_xrefsect, std::string& dest)
{
  tinyxml2::XMLElement* p_xrefsectdescpara = p_xrefsect->FirstChildElement("xrefdescription")->FirstChildElement("para");
  if (p_xrefsectdescpara) // Description has been provided
  {
    XMLTextReader read(dest);
    p_xrefsectdescpara->Accept(&read);
  }
}

} // namespace Parser
