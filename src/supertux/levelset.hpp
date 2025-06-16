//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVELSET_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVELSET_HPP

#include <string>
#include <vector>

class Levelset final
{
private:
  std::string m_basedir;
  std::vector<std::string> m_levels;

public:
  Levelset(const std::string& basedir, bool recursively = false);

  int get_num_levels() const;
  std::string get_level_filename(int i) const;

private:
  Levelset(const Levelset&) = delete;
  Levelset& operator=(const Levelset&) = delete;

  void walk_directory(const std::string& directory, bool recursively = false);
};

#endif

/* EOF */
