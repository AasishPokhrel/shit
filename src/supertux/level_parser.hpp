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

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVEL_PARSER_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVEL_PARSER_HPP

#include <memory>
#include <string>

class Level;
class ReaderDocument;
class ReaderMapping;

class LevelParser final
{
public:
  static std::unique_ptr<Level> from_stream(std::istream& stream, const std::string& context, bool worldmap, bool editable);
  static std::unique_ptr<Level> from_file(const std::string& filename, bool worldmap, bool editable);
  static std::unique_ptr<Level> from_nothing(const std::string& basedir);
  static std::unique_ptr<Level> from_nothing_worldmap(const std::string& basedir, const std::string& name);

  static std::string get_level_name(const std::string& filename);

private:
  LevelParser(Level& level, bool worldmap, bool editable);

  void load(const ReaderDocument& doc);
  void load(std::istream& stream, const std::string& context);
  void load(const std::string& filepath);
  void load_old_format(const ReaderMapping& reader);
  void create(const std::string& filepath, const std::string& levelname);

private:
  Level& m_level;
  bool m_worldmap;
  bool m_editable;

private:
  LevelParser(const LevelParser&) = delete;
  LevelParser& operator=(const LevelParser&) = delete;
};

#endif

/* EOF */
