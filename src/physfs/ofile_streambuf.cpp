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

#include "physfs/ofile_streambuf.hpp"

#include <physfs.h>
#include <sstream>
#include <stdexcept>

#include "physfs/util.hpp"

OFileStreambuf::OFileStreambuf(const std::string& filename) :
  file()
{
  file = PHYSFS_openWrite(filename.c_str());
  if (file == nullptr) {
    std::stringstream msg;
    msg << "Couldn't open file '" << filename << "': "
        << physfsutil::get_last_error();
    throw std::runtime_error(msg.str());
  }

  setp(buf, buf+sizeof(buf));
}

OFileStreambuf::~OFileStreambuf()
{
  sync();
  PHYSFS_close(file);
}

int
OFileStreambuf::overflow(int c)
{
  char c2 = static_cast<char>(c);

  if (pbase() == pptr())
    return 0;

  size_t size = pptr() - pbase();
  PHYSFS_sint64 res = PHYSFS_writeBytes(file, pbase(), size);
  if (res <= 0)
    return traits_type::eof();

  if (c != traits_type::eof()) {
    PHYSFS_sint64 res_ = PHYSFS_writeBytes(file, &c2, 1);
    if (res_ <= 0)
      return traits_type::eof();
  }

  setp(buf, buf + res);
  return 0;
}

int
OFileStreambuf::sync()
{
  return overflow(traits_type::eof());
}

/* EOF */
