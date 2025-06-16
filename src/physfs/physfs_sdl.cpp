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

#include "physfs/physfs_sdl.hpp"

#include <physfs.h>
#include <sstream>
#include <stdexcept>
#include <assert.h>
#include <stdio.h>

#include "physfs/util.hpp"
#include "util/log.hpp"

#include <iostream>

namespace {

Sint64 funcSize(struct SDL_RWops* context)
{
  PHYSFS_file* file = static_cast<PHYSFS_file*>(context->hidden.unknown.data1);
  return PHYSFS_fileLength(file);
}

Sint64 funcSeek(struct SDL_RWops* context, Sint64 offset, int whence)
{
  PHYSFS_file* file = static_cast<PHYSFS_file*>(context->hidden.unknown.data1);
  int res;
  switch (whence) {
    case SEEK_SET:
      res = PHYSFS_seek(file, offset);
      break;
    case SEEK_CUR:
      res = PHYSFS_seek(file, PHYSFS_tell(file) + offset);
      break;
    case SEEK_END:
      res = PHYSFS_seek(file, PHYSFS_fileLength(file) + offset);
      break;
    default:
      res = 0; // NOLINT
      assert(false);
      break;
  }
  if (res == 0) {
    log_warning << "Error seeking in file: " << physfsutil::get_last_error() << std::endl;
    return -1;
  }
  int i = static_cast<int>(PHYSFS_tell(file));


  return i;
}

size_t funcRead(struct SDL_RWops* context, void* ptr, size_t size, size_t maxnum)
{
  PHYSFS_file* file = static_cast<PHYSFS_file*>(context->hidden.unknown.data1);

  PHYSFS_sint64 res = PHYSFS_readBytes(file, ptr, size * maxnum);
  if (res < 0)
  {
    return 0;
  }
  else
  {
    return static_cast<size_t>(res / size);
  }
}

size_t funcWrite(struct SDL_RWops* context, const void* ptr, size_t size, size_t num)
{
  PHYSFS_file* file = static_cast<PHYSFS_file*>(context->hidden.unknown.data1);

  PHYSFS_sint64 res = PHYSFS_writeBytes(file, ptr, size * num);
  if (res < 0)
  {
    return 0;
  }
  else
  {
    return static_cast<size_t>(res / size);
  }
}

int funcClose(struct SDL_RWops* context)
{
  PHYSFS_file* file = static_cast<PHYSFS_file*>(context->hidden.unknown.data1);

  PHYSFS_close(file);
  delete context;

  return 0;
}

} // namespace

SDL_RWops* get_physfs_SDLRWops(const std::string& filename)
{
  // check this as PHYSFS seems to be buggy and still returns a
  // valid pointer in this case
  if (filename.empty()) {
    throw std::runtime_error("Couldn't open file: empty filename");
  }

  PHYSFS_file* file = static_cast<PHYSFS_file*>(PHYSFS_openRead(filename.c_str()));
  if (!file) {
    std::stringstream msg;
    msg << "Couldn't open '" << filename << "': "
        << physfsutil::get_last_error();
    throw std::runtime_error(msg.str());
  }

  SDL_RWops* ops = new SDL_RWops;
  ops->size = funcSize;
  ops->seek = funcSeek;
  ops->read = funcRead;
  ops->write = funcWrite;
  ops->close = funcClose;
  ops->type = SDL_RWOPS_UNKNOWN;
  ops->hidden.unknown.data1 = file;

  return ops;
}

SDL_RWops* get_writable_physfs_SDLRWops(const std::string& filename)
{
  // check this as PHYSFS seems to be buggy and still returns a
  // valid pointer in this case
  if (filename.empty()) {
    throw std::runtime_error("Couldn't open file: empty filename");
  }

  PHYSFS_file* file = static_cast<PHYSFS_file*>(PHYSFS_openWrite(filename.c_str()));
  if (!file) {
    std::stringstream msg;
    msg << "Couldn't open '" << filename << "' for writing: "
        << physfsutil::get_last_error();
    throw std::runtime_error(msg.str());
  }

  SDL_RWops* ops = new SDL_RWops;
  ops->size = funcSize;
  ops->seek = funcSeek;
  ops->read = funcRead;
  ops->write = funcWrite;
  ops->close = funcClose;
  ops->type = SDL_RWOPS_UNKNOWN;
  ops->hidden.unknown.data1 = file;

  return ops;
}

/* EOF */
