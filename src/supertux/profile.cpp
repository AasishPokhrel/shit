//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#include "supertux/profile.hpp"

#include <physfs.h>

#include <sstream>

#include "physfs/util.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

Profile::Profile(int id) :
  m_id(id),
  m_name(),
  m_last_world()
{
  const std::string info_file = get_basedir() + "/info";
  try
  {
    auto doc = ReaderDocument::from_file(info_file);
    auto root = doc.get_root();

    if (root.get_name() != "supertux-profile")
    {
      throw std::runtime_error("File is not a 'supertux-profile' file.");
    }

    auto reader = root.get_mapping();

    reader.get("name", m_name);
    reader.get("last-world", m_last_world);
  }
  catch (const std::exception& err)
  {
    log_info << "Failed to load profile info from '" << info_file << "': " << err.what() << std::endl;
    save();
  }
}

void
Profile::save()
{
  create_basedir();

  Writer writer(get_basedir() + "/info");
  writer.start_list("supertux-profile");

  writer.write("name", m_name);
  writer.write("last-world", m_last_world);

  writer.end_list("supertux-profile");
}

void
Profile::reset()
{
  m_last_world.clear();

  save();
}

void
Profile::create_basedir()
{
  const std::string basedir = get_basedir();
  if (!PHYSFS_exists(basedir.c_str()) && !PHYSFS_mkdir(basedir.c_str()))
  {
    std::ostringstream msg;
    msg << "Couldn't create directory '" << basedir << "' for profile " << m_id << ":"
        << physfsutil::get_last_error();
    throw std::runtime_error(msg.str());
  }
}

std::string
Profile::get_basedir() const
{
  return "profile" + std::to_string(m_id);
}

/* EOF */
