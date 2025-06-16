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

#include "util/reader_object.hpp"

#include <assert.h>
#include <sexp/value.hpp>
#include <stdexcept>

#include "util/reader_collection.hpp"
#include "util/reader_error.hpp"
#include "util/reader_mapping.hpp"

ReaderObject::ReaderObject(const ReaderDocument& doc, const sexp::Value& sx) :
  m_doc(doc),
  m_sx(sx)
{
}

std::string
ReaderObject::get_name() const
{
  assert_array_size_ge(m_doc, m_sx, 1);
  assert_is_symbol(m_doc, m_sx.as_array()[0]);

  return m_sx.as_array()[0].as_string();
}

ReaderMapping
ReaderObject::get_mapping() const
{
  return ReaderMapping(m_doc, m_sx);
}

ReaderCollection
ReaderObject::get_collection() const
{
  return ReaderCollection(m_doc, m_sx);
}

/* EOF */
