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

#ifndef HEADER_SUPERTUX_PHYSFS_IFILE_STREAMBUF_HPP
#define HEADER_SUPERTUX_PHYSFS_IFILE_STREAMBUF_HPP

#include <streambuf>

struct PHYSFS_File;

/** This class implements a C++ streambuf object for physfs files.
 * So that you can use normal istream operations on them
 */
class IFileStreambuf final : public std::streambuf
{
public:
  IFileStreambuf(const std::string& filename);
  ~IFileStreambuf() override;

protected:
  virtual int underflow() override;
  virtual pos_type seekoff(off_type pos, std::ios_base::seekdir,
                           std::ios_base::openmode) override;
  virtual pos_type seekpos(pos_type pos, std::ios_base::openmode) override;

private:
  PHYSFS_File* file;
  char buf[1024];

private:
  IFileStreambuf(const IFileStreambuf&) = delete;
  IFileStreambuf& operator=(const IFileStreambuf&) = delete;
};

#endif

/* EOF */
