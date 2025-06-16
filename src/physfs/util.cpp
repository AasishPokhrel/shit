//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "physfs/util.hpp"

#include <stdexcept>

#include <physfs.h>

#include "physfs/physfs_file_system.hpp"
#include "util/file_system.hpp"

namespace physfsutil {

const char* get_last_error()
{
  return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
}

std::string realpath(const std::string& path)
{
  std::string result = FileSystem::normalize(path);
  if (result.empty()) {
    return "/";
  } else if (result[0] != '/') {
    return '/' + result;
  } else  {
    return result;
  }
}

bool is_directory(const std::string& path)
{
  PHYSFS_Stat statbuf;
  if (!PHYSFS_stat(path.c_str(), &statbuf)) {
    return false;
  } else {
    if (statbuf.filetype == PHYSFS_FILETYPE_SYMLINK) {
      // PHYSFS_stat() doesn't follow symlinks, so we do it manually
      const char* realdir = PHYSFS_getRealDir(path.c_str());
      if (realdir == nullptr) {
        return false;
      } else {
        const std::string realfname = FileSystem::join(realdir, path);
        return FileSystem::is_directory(realfname);
      }
    } else {
      return statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY;
    }
  }
}

bool remove(const std::string& filename)
{
  return PHYSFS_delete(filename.c_str()) == 0;
}

#define PHYSFS_UTIL_DIRECTORY_GUARD \
  if (!is_directory(dir) || !PHYSFS_exists(dir.c_str())) return

void remove_content(const std::string& dir)
{
  PHYSFS_UTIL_DIRECTORY_GUARD;
  enumerate_files(dir, [&dir](const std::string& file) {
    std::string path = FileSystem::join(dir, file);
    if (is_directory(path))
      remove_with_content(path);
    PHYSFS_delete(path.c_str());
    return false;
  });
}

void remove_with_content(const std::string& dir)
{
  PHYSFS_UTIL_DIRECTORY_GUARD;

  remove_content(dir);
  remove(dir);
}

#undef PHYSFS_UTIL_DIRECTORY_GUARD

/* Since PhysFS doesn't provide a way of knowing whether PHYSFS_enumerate has been stopped early
   by the callback via PHYSFS_ENUM_STOP, we have to keep track of whether recursive enumeration
   should stop here, to stop it altogether. */
static bool s_physfs_enumerate_recurse_stop = false;

static PHYSFS_EnumerateCallbackResult physfs_enumerate(void* data, const char*, const char* fname)
{
  auto* callback = static_cast<std::function<bool(const std::string&)>*>(data);
  return callback->operator()(fname) ? PHYSFS_ENUM_STOP : PHYSFS_ENUM_OK;
}

static PHYSFS_EnumerateCallbackResult physfs_enumerate_recurse(void* data, const char* origdir, const char* fname)
{
  const std::string full_path = FileSystem::join(origdir, fname);

  PHYSFS_Stat stat;
  PHYSFS_stat(full_path.c_str(), &stat);
  if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY)
  {
    if (!PHYSFS_enumerate(full_path.c_str(), &physfs_enumerate_recurse, data))
      throw std::runtime_error("Couldn't recursively enumerate directory '" + full_path + "'.");

    if (s_physfs_enumerate_recurse_stop)
      return PHYSFS_ENUM_STOP;
  }
  else
  {
    auto* callback = static_cast<std::function<bool(const std::string&)>*>(data);
    if (callback->operator()(full_path))
    {
      s_physfs_enumerate_recurse_stop = true;
      return PHYSFS_ENUM_STOP;
    }
  }

  return PHYSFS_ENUM_OK;
}

bool enumerate_files(const std::string& pathname, std::function<bool(const std::string&)> callback)
{
  return PHYSFS_enumerate(pathname.c_str(), &physfs_enumerate, &callback);
}

bool enumerate_files_alphabetical(const std::string& pathname, std::function<bool(const std::string&)> callback)
{
  // PHYSFS_enumerateFiles guarantees alphabetical sorting.
  std::unique_ptr<char*, decltype(&PHYSFS_freeList)>
    files(PHYSFS_enumerateFiles(pathname.c_str()),
          PHYSFS_freeList);

  if (!files)
    return false;

  for (const char* const* filename = files.get(); *filename; ++filename)
  {
    if (callback(*filename))
      break;
  }

  return true;
}

bool enumerate_files_recurse(const std::string& pathname, std::function<bool(const std::string&)> callback)
{
  const bool result = PHYSFS_enumerate(pathname.c_str(), &physfs_enumerate_recurse, &callback);
  s_physfs_enumerate_recurse_stop = false;
  return result;
}

} // namespace physfsutil

/* EOF */
