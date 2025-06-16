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

#include "util/reader_collection.hpp"

#include <sexp/value.hpp>

#include "util/reader_error.hpp"

ReaderCollection::ReaderCollection(const ReaderDocument& doc, const sexp::Value& sx) :
  m_doc(doc),
  m_sx(sx)
{
}

std::vector<ReaderObject>
ReaderCollection::get_objects() const
{
  assert_is_array(m_doc, m_sx);

  std::vector<ReaderObject> result;
  auto const& arr = m_sx.as_array();
  for (size_t i = 1; i < arr.size(); ++i)
  {
    result.push_back(ReaderObject(m_doc, arr[i]));
  }
  return result;
}

/* EOF */
