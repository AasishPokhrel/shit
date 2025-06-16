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

#ifndef HEADER_SUPERTUX_UTIL_READER_ERROR_HPP
#define HEADER_SUPERTUX_UTIL_READER_ERROR_HPP

#include <sexp/value.hpp>

#include <sstream>
#include <stdexcept>
#include <sexp/io.hpp>

#include "util/reader_document.hpp"

#define raise_exception(doc, sx, msg) raise_exception_real(__FILE__, __LINE__, doc, sx, msg)

[[noreturn]]
inline void
raise_exception_real(const char* filename, int line,
                     ReaderDocument const& doc, sexp::Value const& sx,
                     const char* usermsg)
{
  std::ostringstream msg;
  msg << "[" << filename << ":" << line << "] "
      << doc.get_filename() << ":" << sx.get_line() << ": "
      << usermsg << " in expression:"
      << "\n    " << sx;
  throw std::runtime_error(msg.str());
}

inline void assert_is_boolean(ReaderDocument const& doc, sexp::Value const& sx)
{
  if (!sx.is_boolean())
  {
    raise_exception(doc, sx, "expected boolean");
  }
}

inline void assert_is_integer(ReaderDocument const& doc, sexp::Value const& sx)
{
  if (!sx.is_integer())
  {
    raise_exception(doc, sx, "expected integer");
  }
}

inline void assert_is_real(ReaderDocument const& doc, sexp::Value const& sx)
{
  if (!sx.is_real())
  {
    raise_exception(doc, sx, "expected real");
  }
}

inline void assert_is_symbol(ReaderDocument const& doc, sexp::Value const& sx)
{
  if (!sx.is_symbol())
  {
    raise_exception(doc, sx, "expected symbol");
  }
}

inline void assert_is_string(ReaderDocument const& doc, sexp::Value const& sx)
{
  if (!sx.is_string())
  {
    raise_exception(doc, sx, "expected string");
  }
}

inline void assert_is_array(ReaderDocument const& doc, sexp::Value const& sx)
{
  if (!sx.is_array())
  {
    raise_exception(doc, sx, "expected array");
  }
}

inline void assert_array_size_ge(ReaderDocument const& doc, sexp::Value const& sx, int size)
{
  assert_is_array(doc, sx);

  if (!(static_cast<int>(sx.as_array().size()) >= size))
  {
    std::ostringstream msg;
    msg << "array should contain " << size << " elements or more";
    raise_exception(doc, sx, msg.str().c_str());
  }
}

inline void assert_array_size_eq(ReaderDocument const& doc, sexp::Value const& sx, int size)
{
  assert_is_array(doc, sx);

  if (static_cast<int>(sx.as_array().size()) != size)
  {
    std::ostringstream msg;
    msg << "array must have " << size << " elements, but has " << sx.as_array().size();
    raise_exception(doc, sx, msg.str().c_str());
  }
}

#endif

/* EOF */
