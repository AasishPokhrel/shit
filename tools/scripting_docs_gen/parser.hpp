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

#ifndef PARSER_HEADER
#define PARSER_HEADER

#include <vector>

#include <tinyxml2.h>

#include "class.hpp"

namespace Parser
{
  void parse_compounddef(tinyxml2::XMLElement* p_root, Class& cls);
  void parse_sectiondef(tinyxml2::XMLElement* p_compounddef, Class& cls);
  void parse_memberdef(tinyxml2::XMLElement* p_sectiondef, Class& cls);

  void parse_constant(tinyxml2::XMLElement* p_memberdef, Class& cls, bool include = false, const std::string& prefix = {});
  void parse_variable(tinyxml2::XMLElement* p_memberdef, Class& cls);
  void parse_function(tinyxml2::XMLElement* p_memberdef, Class& cls);
  void parse_parameterlist(tinyxml2::XMLElement* p_memberdef, Function& func);

  void parse_xrefsect_desc(tinyxml2::XMLElement* p_xrefsect, std::string& dest);
} // namespace Parser

#endif
