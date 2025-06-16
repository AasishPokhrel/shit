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

#ifndef UTIL_HEADER
#define UTIL_HEADER

#include <string>
#include <regex>

#include <tinyxml2.h>

bool param_matches(int argc, char** argv, int i,
                   const std::string& rhs1, const std::string& rhs2,
                   const std::string& rhs3);

bool starts_with(const std::string& str, const std::string& prefix);
void replace(std::string& str, const std::string& from,
             const std::string& to, const std::string& to_if_empty = "");
void regex_replace(std::string& str, const std::regex from,
                   const std::string& to);

std::string read_file(const std::string& path);
void write_file(const std::string& path, const std::string& content);

bool attr_equal(tinyxml2::XMLElement* el, const char* attr, const std::string& rhs);
bool el_equal(tinyxml2::XMLElement* el, const char* child_el, const std::string& rhs);

tinyxml2::XMLElement* first_child_with_attribute(tinyxml2::XMLElement* el,
                                                 const char* child_name,
                                                 const char* child_attr,
                                                 const std::string& child_attr_val);

// XML text reader, which is able to read text from all child nodes.
class XMLTextReader : public tinyxml2::XMLVisitor
{
private:
  std::string& m_buffer;

public:
  XMLTextReader(std::string& buffer) :
    m_buffer(buffer)
  {}

  virtual bool Visit(const tinyxml2::XMLText& txt) override
  {
    const char* val = txt.Value();
    if (val) m_buffer += val;
    return true;
  }
};

#endif
