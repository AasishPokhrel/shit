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

#include "util.hpp"

#include <sstream>
#include <fstream>

bool param_matches(int argc, char** argv, int i,
                   const std::string& rhs1, const std::string& rhs2,
                   const std::string& rhs3) // Allow for 3 parameter formats
{
  const std::string lhs = argv[i];
  return i + 1 < argc && (lhs == rhs1 || lhs == rhs2 || lhs == rhs3);
}


bool starts_with(const std::string& str, const std::string& prefix)
{
  return str.rfind(prefix, 0) == 0;
}

void replace(std::string& str, const std::string& from,
             const std::string& to, const std::string& to_if_empty)
{
  const std::string& to_str = to.empty() ? to_if_empty : to;

  size_t start_pos = str.find(from);
  while (start_pos != std::string::npos)
  {
    str.replace(start_pos, from.length(), to_str);
    start_pos = str.find(from, start_pos + to_str.length());
  }
}

void regex_replace(std::string& str, const std::regex from,
                   const std::string& to)
{
  str = std::regex_replace(str, from, to);
}


std::string read_file(const std::string& path)
{
  std::ifstream stream(path);
  std::stringstream buffer;
  buffer << stream.rdbuf();
  stream.close();
  return buffer.str();
}

void write_file(const std::string& path, const std::string& content)
{
  std::ofstream stream(path);
  stream << content;
  stream.close();
}


bool attr_equal(tinyxml2::XMLElement* el, const char* attr, const std::string& rhs)
{
  const tinyxml2::XMLAttribute* attr_obj = el->FindAttribute(attr);
  if (!attr_obj) return false;

  const char* val = attr_obj->Value();
  return val == NULL ? rhs.empty() : !strcmp(val, rhs.c_str());
}

bool el_equal(tinyxml2::XMLElement* el, const char* child_el, const std::string& rhs)
{
  const tinyxml2::XMLElement* child_el_obj = el->FirstChildElement(child_el);
  if (!child_el_obj) return false;

  const char* text = child_el_obj->GetText();
  return text == NULL ? rhs.empty() : !strcmp(text, rhs.c_str());
}


tinyxml2::XMLElement* first_child_with_attribute(tinyxml2::XMLElement* el,
                                                 const char* child_name,
                                                 const char* child_attr,
                                                 const std::string& child_attr_val)
{
  tinyxml2::XMLElement* child_el = el->FirstChildElement(child_name);
  while (child_el)
  {
    if (attr_equal(child_el, child_attr, child_attr_val))
      return child_el;

    child_el = child_el->NextSiblingElement(child_name);
  }
  return nullptr;
}
