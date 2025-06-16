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

#include "supertux/world.hpp"

#include <physfs.h>
#include <sstream>

#include "physfs/util.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

std::unique_ptr<World>
World::from_directory(const std::string& directory)
{
  std::unique_ptr<World> world(new World(directory));

  std::string info_filename = FileSystem::join(directory, "info");

  try
  {
    register_translation_directory(info_filename);
    
    if (!PHYSFS_exists(info_filename.c_str()) && 
        !FileSystem::exists(info_filename))
    {
      world->m_hide_from_contribs = true;
      return world;
    }

    auto doc = ReaderDocument::from_file(info_filename);
    auto root = doc.get_root();

    if (root.get_name() != "supertux-world" &&
        root.get_name() != "supertux-level-subset")
    {
      throw std::runtime_error("File is not a world or levelsubset file");
    }

    auto info = root.get_mapping();

    info.get("title", world->m_title);
    info.get("description", world->m_description);
    info.get("levelset", world->m_is_levelset, true);
    info.get("hide-from-contribs", world->m_hide_from_contribs, false);
    info.get("contrib-type", world->m_contrib_type, "user");
    info.get("title-level", world->m_title_level);
  }
  catch (const std::exception& err)
  {
    log_warning << "Failed to load " << info_filename << ":" << err.what() << std::endl;

    world->m_hide_from_contribs = true;
  }

  return world;
}

std::unique_ptr<World>
World::create(const std::string& title, const std::string& desc)
{
  // Limit the charset to numbers and alphabet.
  std::string base = title;

  for (size_t i = 0; i < base.length(); i++) {
    if (!isalnum(base[i])) {
      base[i] = '_';
    }
  }

  base = FileSystem::join("levels", base);

  // Find a non-existing fitting directory name
  std::string dirname = base;
  if (PHYSFS_exists(dirname.c_str())) {
    int num = 1;
    do {
      num++;
      dirname = base + std::to_string(num);
    } while (PHYSFS_exists(dirname.c_str()));
  }

  std::unique_ptr<World> world(new World(dirname));

  world->m_title = title;
  world->m_description = desc;

  return world;
}

World::World(const std::string& directory) :
  m_title(),
  m_description(),
  m_is_levelset(true),
  m_basedir(directory),
  m_hide_from_contribs(false),
  m_contrib_type(),
  m_title_level()
{
}

std::string
World::get_basename() const
{
  return FileSystem::basename(m_basedir);
}

void
World::save(bool retry)
{
  std::string filepath = FileSystem::join(m_basedir, "/info");

  try
  {
    { // make sure the levelset directory exists
      std::string dirname = FileSystem::dirname(filepath);
      if (!PHYSFS_exists(dirname.c_str()))
      {
        if (!PHYSFS_mkdir(dirname.c_str()))
        {
          std::ostringstream msg;
          msg << "Couldn't create directory for levelset '"
              << dirname << "': " <<physfsutil::get_last_error();
          throw std::runtime_error(msg.str());
        }
      }

      if (!physfsutil::is_directory(dirname))
      {
        std::ostringstream msg;
        msg << "Levelset path '" << dirname << "' is not a directory";
        throw std::runtime_error(msg.str());
      }
    }

    Writer writer(filepath);
    writer.start_list("supertux-level-subset");

    writer.write("title", m_title, true);
    writer.write("description", m_description, true);
    writer.write("levelset", m_is_levelset);
    writer.write("contrib-type", "user");
    writer.write("hide-from-contribs", m_hide_from_contribs);
    writer.write("title-level", m_title_level);

    writer.end_list("supertux-level-subset");
    log_warning << "Levelset info saved as " << filepath << "." << std::endl;
  }
  catch(std::exception& e)
  {
    if (retry) {
      std::stringstream msg;
      msg << "Problem when saving levelset info '" << filepath << "': " << e.what();
      throw std::runtime_error(msg.str());
    } else {
      log_warning << "Failed to save the levelset info, retrying..." << std::endl;
      { // create the levelset directory again
        std::string dirname = FileSystem::dirname(filepath);
        if (!PHYSFS_mkdir(dirname.c_str()))
        {
          std::ostringstream msg;
          msg << "Couldn't create directory for levelset '"
              << dirname << "': " <<physfsutil::get_last_error();
          throw std::runtime_error(msg.str());
        }
      }
      save(true);
    }
  }
}

std::string
World::get_worldmap_filename() const
{
  return FileSystem::join(m_basedir, "worldmap.stwm");
}

/* EOF */
