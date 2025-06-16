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

#include "supertux/level_parser.hpp"

#include <physfs.h>
#include <sstream>

#include "supertux/constants.hpp"
#include "supertux/level.hpp"
#include "supertux/sector.hpp"
#include "supertux/sector_parser.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"

std::string
LevelParser::get_level_name(const std::string& filename)
{
  try
  {
    register_translation_directory(filename);
    auto doc = ReaderDocument::from_file(filename, 1);
    auto root = doc.get_root();

    if (root.get_name() != "supertux-level") {
      return "";
    } else {
      auto mapping = root.get_mapping();
      std::string name;
      mapping.get("name", name);
      return name;
    }
  }
  catch(const std::exception& e)
  {
    log_warning << "Problem getting name of '" << filename << "': "
                << e.what() << std::endl;
    return "";
  }
}

std::unique_ptr<Level>
LevelParser::from_stream(std::istream& stream, const std::string& context, bool worldmap, bool editable)
{
  auto level = std::make_unique<Level>(worldmap);
  LevelParser parser(*level, worldmap, editable);
  parser.load(stream, context);
  return level;
}

std::unique_ptr<Level>
LevelParser::from_file(const std::string& filename, bool worldmap, bool editable)
{
  auto level = std::make_unique<Level>(worldmap);
  LevelParser parser(*level, worldmap, editable);
  parser.load(filename);
  return level;
}

std::unique_ptr<Level>
LevelParser::from_nothing(const std::string& basedir)
{
  auto level = std::make_unique<Level>(false);
  LevelParser parser(*level, false, false);

  // Find a free level filename
  std::string level_file;
  int num = 0;
  do {
    num++;
    level_file = basedir + "/level" + std::to_string(num) + ".stl";
  } while ( PHYSFS_exists(level_file.c_str()) );
  std::string level_name = "Level " + std::to_string(num);
  level_file = "level" + std::to_string(num) + ".stl";

  parser.create(level_file, level_name);
  return level;
}

std::unique_ptr<Level>
LevelParser::from_nothing_worldmap(const std::string& basedir, const std::string& name)
{
  auto level = std::make_unique<Level>(true);
  LevelParser parser(*level, true, false);

  // Find a free level filename
  std::string level_file = basedir + "/worldmap.stwm";
  if (PHYSFS_exists(level_file.c_str())) {
    int num = 0;
    do {
      num++;
      level_file = basedir + "/worldmap" + std::to_string(num) + ".stwm";
    } while ( PHYSFS_exists(level_file.c_str()) );
    level_file = "worldmap" + std::to_string(num) + ".stwm";
  } else {
    level_file = "worldmap.stwm";
  }

  parser.create(level_file, name);
  return level;
}

LevelParser::LevelParser(Level& level, bool worldmap, bool editable) :
  m_level(level),
  m_worldmap(worldmap),
  m_editable(editable)
{
}

void
LevelParser::load(std::istream& stream, const std::string& context)
{
  auto doc = ReaderDocument::from_stream(stream, context);
  load(doc);
}

void
LevelParser::load(const std::string& filepath)
{
  m_level.m_filename = filepath;
  register_translation_directory(filepath);
  try {
    auto doc = ReaderDocument::from_file(filepath);
    load(doc);
  } catch(std::exception& e) {
    std::stringstream msg;
    msg << "Problem when reading level '" << filepath << "': " << e.what();
    throw std::runtime_error(msg.str());
  }
}

void
LevelParser::load(const ReaderDocument& doc)
{
  auto root = doc.get_root();

  if (root.get_name() != "supertux-level")
    throw std::runtime_error("file is not a supertux-level file.");

  auto level = root.get_mapping();

  int version = 1;
  level.get("version", version);
  if (version == 1)
  {
    log_info << "[" << doc.get_filename() << "] level uses old format: version 1" << std::endl;
    load_old_format(level);
  }
  else if (version == 2 || version == 3)
  {
    level.get("tileset", m_level.m_tileset);

    level.get("name", m_level.m_name);
    level.get("author", m_level.m_author);
    level.get("contact", m_level.m_contact);
    level.get("license", m_level.m_license);
    level.get("target-time", m_level.m_target_time);
    level.get("suppress-pause-menu", m_level.m_suppress_pause_menu);
    level.get("note", m_level.m_note);
    level.get("icon", m_level.m_icon);
    level.get("icon-locked", m_level.m_icon_locked);
    level.get("bkg", m_level.m_wmselect_bkg);

    std::string name = m_level.m_is_worldmap ? "on" : "inherit";
    level.get("allow-item-pocket", name);
    m_level.m_allow_item_pocket = Level::get_setting_from_name(name);

    std::optional<ReaderMapping> level_stat_preferences;
    if (level.get("statistics", level_stat_preferences))
      m_level.m_stats.get_preferences().parse(*level_stat_preferences);

    auto iter = level.get_iter();
    while (iter.next())
    {
      if (iter.get_key() == "sector")
      {
        auto sector = SectorParser::from_reader(m_level, iter.as_mapping(), m_editable);
        m_level.add_sector(std::move(sector));
      }
    }

    if (m_level.m_license.empty()) {
      log_warning << "[" <<  doc.get_filename() << "] The level author \"" << m_level.m_author
                  << "\" did not specify a license for this level \""
                  << m_level.m_name << "\". You might not be allowed to share it."
                  << std::endl;
    }
  }
  else
  {
    log_warning << "[" << doc.get_filename() << "] level format version " << version << " is not supported" << std::endl;
  }

  m_level.initialize();
}

void
LevelParser::load_old_format(const ReaderMapping& reader)
{
  reader.get("name", m_level.m_name);
  reader.get("author", m_level.m_author);

  auto sector = SectorParser::from_reader_old_format(m_level, reader, m_editable);
  m_level.add_sector(std::move(sector));
}

void
LevelParser::create(const std::string& filepath, const std::string& levelname)
{
  m_level.m_filename = filepath;
  m_level.m_name = levelname;
  m_level.m_license = "CC-BY-SA 4.0 International";
  m_level.m_tileset = m_worldmap ? "images/ice_world.strf" : "images/tiles.strf";

  auto sector = SectorParser::from_nothing(m_level);
  sector->set_name(DEFAULT_SECTOR_NAME);
  m_level.add_sector(std::move(sector));
}

/* EOF */
