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

#ifndef HEADER_SUPERTUX_UTIL_READER_DOCUMENT_HPP
#define HEADER_SUPERTUX_UTIL_READER_DOCUMENT_HPP

#include <istream>
#include <sexp/value.hpp>

#include "util/reader_object.hpp"

/** The ReaderDocument holds a parsed document in memory, access to
    it's content is provided by get_root() */
class ReaderDocument final
{
public:
  static ReaderDocument from_string(const std::string& string, const std::string& filename = "<string>", int depth = -1);
  static ReaderDocument from_stream(std::istream& stream, const std::string& filename = "<stream>", int depth = -1);
  static ReaderDocument from_file(const std::string& filename, int depth = -1);

public:
  ReaderDocument(const std::string& filename, sexp::Value sx);

  /** Returns the root object */
  ReaderObject get_root() const;

  /** Returns the filename of the document */
  const std::string& get_filename() const;

  /** Returns the directory of the document */
  std::string get_directory() const;

  inline const sexp::Value& get_sexp() const { return m_sx; }

private:
  std::string m_filename;
  sexp::Value m_sx;
};

#endif

/* EOF */
