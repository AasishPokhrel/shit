//  SuperTux
//  Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_UTIL_READER_MAPPING_HPP
#define HEADER_SUPERTUX_UTIL_READER_MAPPING_HPP

#include <cstdint>
#include <optional>

#include "util/reader_iterator.hpp"
#include "util/uid.hpp"

namespace sexp {
class Value;
} // namespace sexp

class ReaderDocument;
class ReaderCollection;

class ReaderMapping final
{
public:
  static bool s_translations_enabled;

public:
  // sx should point to (section (name value)...)
  ReaderMapping(const ReaderDocument& doc, const sexp::Value& sx);

  ReaderIterator get_iter() const;

  bool get(const char* key, bool& value, const std::optional<bool>& default_value = std::nullopt) const;
  bool get(const char* key, int& value, const std::optional<int>& default_value = std::nullopt) const;
  bool get(const char* key, uint32_t& value, const std::optional<uint32_t>& default_value = std::nullopt) const;
  bool get(const char* key, UID& value, const std::optional<UID>& default_value = std::nullopt) const;
  bool get(const char* key, float& value, const std::optional<float>& default_value = std::nullopt) const;
  bool get(const char* key, std::string& value, const std::optional<const char*>& default_value = std::nullopt) const;

  bool get(const char* key, std::vector<bool>& value, const std::optional<std::vector<bool>>& default_value = std::nullopt) const;
  bool get(const char* key, std::vector<int>& value, const std::optional<std::vector<int>>& default_value = std::nullopt) const;
  bool get(const char* key, std::vector<float>& value, const std::optional<std::vector<float>>& default_value = std::nullopt) const;
  bool get(const char* key, std::vector<std::string>& value, const std::optional<std::vector<std::string>>& default_value = std::nullopt) const;
  bool get(const char* key, std::vector<unsigned int>& value, const std::optional<std::vector<unsigned int>>& default_value = std::nullopt) const;

  // Reads vector by using the absolute value of any negative integer value as a repeater for the next value.
  bool get_compressed(const char* key, std::vector<unsigned int>& value,
                      const std::optional<std::vector<unsigned int>>& default_value = std::nullopt) const;

  bool get(const char* key, std::optional<ReaderMapping>&) const;
  bool get(const char* key, std::optional<ReaderCollection>&) const;

  bool get(const char* key, sexp::Value& value) const;

  /** Read a custom data format, such an as enum. The data is stored
      as string and converted to the custom type using the supplied
      `from_string` convert function. Example:

      mapping.get_custom("style", value, Style_from_string, Style::DEFAULT); */
  template<typename C, typename F>
  bool get_custom(const char* key, C& value, F from_string, std::optional<decltype(C())> default_value = std::nullopt) const
  {
    std::string text;
    if (!get(key, text))
    {
      if (default_value) {
        value = *default_value;
      }
      return false;
    }
    else
    {
      value = from_string(text);
      return true;
    }
  }

  inline const sexp::Value& get_sexp() const { return m_sx; }
  inline const ReaderDocument& get_doc() const { return m_doc; }

private:
  /** Returns pointer to (key value) */
  const sexp::Value* get_item(const char* key) const;

private:
  const ReaderDocument& m_doc;
  const sexp::Value& m_sx;
  const std::vector<sexp::Value>& m_arr;
};

#endif

/* EOF */
