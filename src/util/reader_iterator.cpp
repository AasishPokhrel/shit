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

#include "util/reader_iterator.hpp"

#include <sexp/io.hpp>
#include <sstream>

#include "util/reader_error.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"

ReaderIterator::ReaderIterator(const ReaderDocument& doc, const sexp::Value& sx) :
  m_doc(doc),
  m_arr(sx.as_array()),
  m_idx(0)
{
}

bool
ReaderIterator::next()
{
  m_idx += 1;
  return m_idx < m_arr.size();
}

bool
ReaderIterator::is_string()
{
  return m_arr[m_idx].is_string();
}

bool
ReaderIterator::is_pair()
{
  return m_arr[m_idx].is_array();
}

std::string
ReaderIterator::as_string_item()
{
  assert_is_string(m_doc, m_arr[m_idx]);

  return m_arr[m_idx].as_string();
}

std::string
ReaderIterator::get_key() const
{
  assert_is_array(m_doc, m_arr[m_idx]);
  assert_array_size_ge(m_doc, m_arr[m_idx], 1);

  return m_arr[m_idx].as_array()[0].as_string();
}

void
ReaderIterator::get(bool& value) const
{
  assert_is_array(m_doc, m_arr[m_idx]);
  assert_array_size_eq(m_doc, m_arr[m_idx], 2);
  assert_is_boolean(m_doc, m_arr[m_idx].as_array()[1]);

  value = m_arr[m_idx].as_array()[1].as_bool();
}

void
ReaderIterator::get(int& value) const
{
  assert_is_array(m_doc, m_arr[m_idx]);
  assert_array_size_eq(m_doc, m_arr[m_idx], 2);
  assert_is_integer(m_doc, m_arr[m_idx].as_array()[1]);

  value = m_arr[m_idx].as_array()[1].as_int();
}

void
ReaderIterator::get(float& value) const
{
  assert_is_array(m_doc, m_arr[m_idx]);
  assert_array_size_eq(m_doc, m_arr[m_idx], 2);
  assert_is_real(m_doc, m_arr[m_idx].as_array()[1]);

  value = m_arr[m_idx].as_array()[1].as_float();
}

void
ReaderIterator::get(std::string& value) const
{
  assert_is_array(m_doc, m_arr[m_idx]);
  assert_array_size_eq(m_doc, m_arr[m_idx], 2);
  assert_is_string(m_doc, m_arr[m_idx].as_array()[1]);

  value = m_arr[m_idx].as_array()[1].as_string();
}

ReaderMapping
ReaderIterator::as_mapping() const
{
  return ReaderMapping(m_doc, m_arr[m_idx]);
}

const sexp::Value&
ReaderIterator::get_sexp() const
{
  return m_arr[m_idx];
}

/* EOF */
