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

#include "squirrel/serialize.hpp"

#include <sexp/value.hpp>
#include <sexp/util.hpp>
#include <simplesquirrel/table.hpp>

#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

void load_squirrel_table(ssq::Table& table, const ReaderMapping& mapping)
{
  auto const& arr = mapping.get_sexp().as_array();
  for (size_t i = 1; i < arr.size(); ++i)
  {
    const auto& pair = arr[i].as_array();

    // Ignore key value pairs with invalid length
    if (pair.size() < 2)
    {
      log_debug << "Found key/value pair with invalid length. Ignoring." << std::endl;
      continue;
    }

    const char* key = pair[0].as_string().c_str();
    const auto& value = pair[1];

    // ignore empty / null values
    if (value.is_nil())
    {
      log_debug << "Found null value for key " << key << ". Ignoring." << std::endl;
      continue;
    }

    // push the value
    switch (value.get_type())
    {
      case sexp::Value::Type::ARRAY:
      {
        ssq::Table new_table = table.addTable(key);
        load_squirrel_table(new_table, ReaderMapping(mapping.get_doc(), arr[i]));
        break;
      }
      case sexp::Value::Type::INTEGER:
        table.set(key, value.as_int());
        break;
      case sexp::Value::Type::REAL:
        table.set(key, value.as_float());
        break;
      case sexp::Value::Type::STRING:
        table.set(key, value.as_string());
        break;
      case sexp::Value::Type::BOOLEAN:
        table.set(key, value.as_bool());
        break;
      case sexp::Value::Type::SYMBOL:
        log_warning << "Unexpected symbol in file: " << value.as_string() << std::endl;
        table.set(key, nullptr);
        break;
      default:
        assert(false);
        break;
    }
  }
}

void save_squirrel_table(const ssq::Table& table, Writer& writer)
{
  for (const auto& [key, value] : table.convertRaw())
  {
    switch (value.getType())
    {
      case ssq::Type::INTEGER:
        writer.write(key, value.to<int>());
        break;
      case ssq::Type::FLOAT:
        writer.write(key, value.toFloat());
        break;
      case ssq::Type::BOOL:
        writer.write(key, value.toBool());
        break;
      case ssq::Type::STRING:
        writer.write(key, value.toString());
        break;
      case ssq::Type::TABLE:
        writer.start_list(key, true);
        save_squirrel_table(value.toTable(), writer);
        writer.end_list(key);
        break;

      case ssq::Type::CLOSURE:
      case ssq::Type::NATIVECLOSURE:
        break; // Ignore

      default:
        log_warning << "Can't serialize key '" << key << "' in Squirrel table." << std::endl;
        break;
    }
  }
}

/* EOF */
