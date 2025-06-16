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

#ifndef HEADER_SUPERTUX_UTIL_READER_COLLECTION_HPP
#define HEADER_SUPERTUX_UTIL_READER_COLLECTION_HPP

#include <vector>

#include "util/reader_object.hpp"

namespace sexp {
class Value;
} // namespace sexp

class ReaderDocument;

class ReaderCollection final
{
public:
  /** sx should point to (section (objname (name value)...)...) */
  ReaderCollection(const ReaderDocument& doc, const sexp::Value& sx);

  std::vector<ReaderObject> get_objects() const;

  inline const ReaderDocument& get_doc() const { return m_doc; }
  inline const sexp::Value& get_sexp() const { return m_sx; }

private:
  const ReaderDocument& m_doc;
  const sexp::Value& m_sx;
};

#endif

/* EOF */
