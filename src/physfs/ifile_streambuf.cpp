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

#include "physfs/ifile_streambuf.hpp"

#include <assert.h>
#include <physfs.h>
#include <sstream>
#include <stdexcept>

#include "physfs/util.hpp"

IFileStreambuf::IFileStreambuf(const std::string& filename) :
  file(),
  buf()
{
  // Check this as PHYSFS seems to be buggy and still returns a
  // valid pointer in this case.
  if (filename.empty()) {
    throw std::runtime_error("Couldn't open file: empty filename");
  }
  file = PHYSFS_openRead(filename.c_str());
  if (file == nullptr) {
    std::stringstream msg;
    msg << "Couldn't open file '" << filename << "': "
        << physfsutil::get_last_error();
    throw std::runtime_error(msg.str());
  }
}

IFileStreambuf::~IFileStreambuf()
{
  PHYSFS_close(file);
}

int
IFileStreambuf::underflow()
{
  if (PHYSFS_eof(file)) {
    return traits_type::eof();
  }

  PHYSFS_sint64 bytesread = PHYSFS_readBytes(file, buf, sizeof(buf));
  if (bytesread <= 0) {
    return traits_type::eof();
  }
  setg(buf, buf, buf + bytesread);

  return static_cast<unsigned char>(buf[0]);
}

IFileStreambuf::pos_type
IFileStreambuf::seekpos(pos_type pos, std::ios_base::openmode)
{
  if (PHYSFS_seek(file, static_cast<PHYSFS_uint64> (pos)) == 0) {
    return pos_type(off_type(-1));
  }

  // The seek invalidated the buffer.
  setg(buf, buf, buf);
  return pos;
}

IFileStreambuf::pos_type
IFileStreambuf::seekoff(off_type off, std::ios_base::seekdir dir,
                        std::ios_base::openmode mode)
{
  off_type pos = off;
  PHYSFS_sint64 ptell = PHYSFS_tell(file);

  switch (dir) {
    case std::ios_base::beg:
      break;
    case std::ios_base::cur:
      if (off == 0)
        return static_cast<pos_type> (ptell) - static_cast<pos_type> (egptr() - gptr());
      pos += static_cast<off_type> (ptell) - static_cast<off_type> (egptr() - gptr());
      break;
    case std::ios_base::end:
      pos += static_cast<off_type> (PHYSFS_fileLength(file));
      break;
    default:
      assert(false);
      return pos_type(off_type(-1));
  }

  return seekpos(static_cast<pos_type> (pos), mode);
}

/* EOF */
