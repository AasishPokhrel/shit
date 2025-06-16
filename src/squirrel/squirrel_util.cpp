//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "squirrel/squirrel_util.hpp"

#include <simplesquirrel/array.hpp>
#include <simplesquirrel/table.hpp>

std::string squirrel_to_string(const ssq::Object& object)
{
  std::ostringstream os;
  switch (object.getType())
  {
    case ssq::Type::NULLPTR:
      os << "<null>";
      break;
    case ssq::Type::BOOL:
      os << object.toBool();
      break;
    case ssq::Type::INTEGER:
      os << object.to<int>();
      break;
    case ssq::Type::FLOAT:
      os << object.toFloat();
      break;
    case ssq::Type::STRING:
      os << "\"" << object.toString() << "\"";
      break;
    case ssq::Type::TABLE:
    {
      const std::map<std::string, ssq::Object> table = object.toTable().convertRaw();

      bool first = true;
      os << "{";
      for (const auto& [key, value] : table)
      {
        if (!first)
          os << ", ";
        first = false;

        os << key << " => " << squirrel_to_string(value);
      }
      os << "}";
      break;
    }
    case ssq::Type::ARRAY:
    {
      const std::vector<ssq::Object> array = object.toArray().convertRaw();

      bool first = true;
      os << "[";
      for (const ssq::Object& value : array)
      {
        if (!first)
          os << ", ";
        first = false;

        os << squirrel_to_string(value);
      }
      os << "]";
      break;
    }
    case ssq::Type::USERDATA:
      os << "<userdata>";
      break;
    case ssq::Type::CLOSURE:
      os << "<closure>";
      break;
    case ssq::Type::NATIVECLOSURE:
      os << "<native closure>";
      break;
    case ssq::Type::GENERATOR:
      os << "<generator>";
      break;
    case ssq::Type::USERPOINTER:
      os << "userpointer";
      break;
    case ssq::Type::THREAD:
      os << "<thread>";
      break;
    case ssq::Type::CLASS:
      os << "<class>";
      break;
    case ssq::Type::INSTANCE:
      os << "<instance>";
      break;
    case ssq::Type::WEAKREF:
      os << "<weakref>";
      break;
    default:
      os << "<unknown>";
      break;
  }
  return os.str();
}

/* EOF */
