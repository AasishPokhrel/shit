//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "util/utf8_iterator.hpp"

#include <stdexcept>

#include "util/log.hpp"

namespace {

bool     has_multibyte_mark(unsigned char c);
uint32_t decode_utf8(const std::string& text, size_t& p);

/**
 * returns true if this byte matches a bitmask of 10xx.xxxx, i.e. it is the 2nd, 3rd or 4th byte of a multibyte utf8 string
 */
bool has_multibyte_mark(unsigned char c) {
  return ((c & 0300) == 0200);
}

/**
 * gets unicode character at byte position @a p of UTF-8 encoded @a
 * text, then advances @a p to the next character.
 *
 * @throws std::runtime_error if decoding fails.
 * See unicode standard section 3.10 table 3-5 and 3-6 for details.
 */
uint32_t decode_utf8(const std::string& text, size_t& p)
{
  uint32_t c1 = static_cast<unsigned char>(text[p+0]);

  if (has_multibyte_mark(static_cast<unsigned char>(c1))) {
    throw std::runtime_error("Malformed utf-8 sequence");
  }

  if ((c1 & 0200) == 0000) {
    // 0xxx.xxxx: 1 byte sequence
    p+=1;
    return c1;
  }
  else if ((c1 & 0340) == 0300) {
    // 110x.xxxx: 2 byte sequence
    if (p+1 >= text.size()) throw std::range_error("Malformed utf-8 sequence");
    uint32_t c2 = static_cast<unsigned char>(text[p+1]);
    if (!has_multibyte_mark(static_cast<unsigned char>(c2))) throw std::runtime_error("Malformed utf-8 sequence");
    p+=2;
    return (c1 & 0037) << 6 | (c2 & 0077);
  }
  else if ((c1 & 0360) == 0340) {
    // 1110.xxxx: 3 byte sequence
    if (p+2 >= text.size()) throw std::range_error("Malformed utf-8 sequence");
    uint32_t c2 = static_cast<unsigned char>(text[p+1]);
    uint32_t c3 = static_cast<unsigned char>(text[p+2]);
    if (!has_multibyte_mark(static_cast<unsigned char>(c2))) throw std::runtime_error("Malformed utf-8 sequence");
    if (!has_multibyte_mark(static_cast<unsigned char>(c3))) throw std::runtime_error("Malformed utf-8 sequence");
    p+=3;
    return (c1 & 0017) << 12 | (c2 & 0077) << 6 | (c3 & 0077);
  }
  else if ((c1 & 0370) == 0360) {
    // 1111.0xxx: 4 byte sequence
    if (p+3 >= text.size()) throw std::range_error("Malformed utf-8 sequence");
    uint32_t c2 = static_cast<unsigned char>(text[p+1]);
    uint32_t c3 = static_cast<unsigned char>(text[p+2]);
    uint32_t c4 = static_cast<unsigned char>(text[p+4]);
    if (!has_multibyte_mark(static_cast<unsigned char>(c2))) throw std::runtime_error("Malformed utf-8 sequence");
    if (!has_multibyte_mark(static_cast<unsigned char>(c3))) throw std::runtime_error("Malformed utf-8 sequence");
    if (!has_multibyte_mark(static_cast<unsigned char>(c4))) throw std::runtime_error("Malformed utf-8 sequence");
    p+=4;
    return (c1 & 0007) << 18 | (c2 & 0077) << 12 | (c3 & 0077) << 6 | (c4 & 0077);
  }
  throw std::runtime_error("Malformed utf-8 sequence");
}

} // namespace


UTF8Iterator::UTF8Iterator(const std::string& text_) :
  text(text_),
  pos(0),
  chr()
{
  try {
    chr = decode_utf8(text, pos);
  } catch (std::exception&) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
    log_debug << "Malformed utf-8 sequence beginning with " << *(reinterpret_cast<const uint32_t*>(text.c_str() + pos)) << " found " << std::endl;
#pragma GCC diagnostic pop
    chr = 0;
  }
}

  bool
UTF8Iterator::done() const
  {
    return pos > text.size();
  }

  UTF8Iterator&
UTF8Iterator::operator++() {
    try {
      chr = decode_utf8(text, pos);
    } catch (std::exception&) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
      log_debug << "Malformed utf-8 sequence beginning with " << *(reinterpret_cast<const uint32_t*>(text.c_str() + pos)) << " found " << std::endl;
#pragma GCC diagnostic pop
      chr = 0;
      ++pos;
    }

    return *this;
  }

  uint32_t
  UTF8Iterator::operator*() const {
    return chr;
  }

/* EOF */
