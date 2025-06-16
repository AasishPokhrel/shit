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

#include "physfs/physfs_file_system.hpp"

#include <physfs.h>

#include "physfs/ifile_stream.hpp"
#include "physfs/util.hpp"

PhysFSFileSystem::PhysFSFileSystem()
{
}

std::vector<std::string>
PhysFSFileSystem::open_directory(const std::string& pathname)
{
  std::vector<std::string> files;
  physfsutil::enumerate_files_alphabetical(pathname, [&files](const std::string& file) {
    files.push_back(file);
    return false;
  });
  return files;
}

std::unique_ptr<std::istream>
PhysFSFileSystem::open_file(const std::string& filename)
{
  return std::make_unique<IFileStream>(filename);
}

/* EOF */
