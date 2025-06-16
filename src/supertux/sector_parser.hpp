//  SuperTux
//  Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_SUPERTUX_SECTOR_PARSER_HPP
#define HEADER_SUPERTUX_SUPERTUX_SECTOR_PARSER_HPP

#include <memory>
#include <string>

class GameObject;
class Level;
class ReaderMapping;
class Sector;

namespace Base {
  class Sector;
}

class SectorParser
{
public:
  static std::unique_ptr<Sector> from_reader(Level& level, const ReaderMapping& sector, bool editable);
  static std::unique_ptr<Sector> from_reader_old_format(Level& level, const ReaderMapping& sector, bool editable);
  static std::unique_ptr<Sector> from_nothing(Level& level);

protected:
  SectorParser(Base::Sector& sector, bool editable);
  virtual ~SectorParser() {}

  void parse_old_format(const ReaderMapping& reader);
  void parse(const ReaderMapping& reader);
  void create_sector();

  std::unique_ptr<GameObject> parse_object(const std::string& name, const ReaderMapping& reader);

  /** Allows setting additional rules for parsing objects.
      Return value indicates whether the regular object parsing process should be skipped. **/
  virtual bool parse_object_additional(const std::string& name, const ReaderMapping& reader);

protected:
  Base::Sector& m_sector;
  bool m_editable;

private:
  SectorParser(const SectorParser&) = delete;
  SectorParser& operator=(const SectorParser&) = delete;
};

#endif

/* EOF */
