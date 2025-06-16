//  SuperTux - Add-on
//  Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
//                2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "addon/addon.hpp"

#include <optional>
#include <fmt/format.h>
#include <sstream>

#include "util/gettext.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_mapping.hpp"

static const char* s_allowed_characters = "-_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

namespace addon_string_util {

Addon::Type addon_type_from_string(const std::string& type)
{
  if (type == "world")
  {
    return Addon::WORLD;
  }
  else if (type == "worldmap")
  {
    return Addon::WORLDMAP;
  }
  else if (type == "levelset")
  {
    return Addon::LEVELSET;
  }
  else if (type == "languagepack")
  {
    return Addon::LANGUAGEPACK;
  }
  else if (type == "resourcepack")
  {
    return Addon::RESOURCEPACK;
  }
  else if (type == "weakresourcepack")
  {
    return Addon::WEAKRESOURCEPACK;
  }
  else if (type == "addon")
  {
    return Addon::ADDON;
  }
  else
  {
    throw std::runtime_error("Not a valid Addon::Type: " + type);
  }
}

std::string addon_type_to_translated_string(Addon::Type type)
{
  switch (type)
  {
    case Addon::LEVELSET:
      return _("Levelset");

    case Addon::WORLDMAP:
      return _("Worldmap");

    case Addon::WORLD:
      return _("World");

    case Addon::ADDON:
      return _("Add-on");

    case Addon::LANGUAGEPACK:
      return _("Language Pack");

    case Addon::RESOURCEPACK:
    case Addon::WEAKRESOURCEPACK:
      return _("Resource Pack");

    default:
      return _("Unknown");
  }
}

std::string generate_menu_item_text(const Addon& addon)
{
  std::string text;
  std::string type = addon_type_to_translated_string(addon.get_type());

  if (!addon.get_author().empty())
  {
    text = fmt::format(fmt::runtime(_("{} \"{}\" by \"{}\"")),
                       type, addon.get_title(), addon.get_author());
  }
  else
  {
    // Only add-on type and name, no need for translation.
    text = fmt::format("{} \"{}\"", type, addon.get_title());
  }

  return text;
}

std::string get_addon_plural_form(size_t count)
{
  return (count == 1 ? _("add-on") : _("add-ons"));
}

} // namespace addon_string_util

std::unique_ptr<Addon>
Addon::parse(const ReaderMapping& mapping)
{
  std::unique_ptr<Addon> addon(new Addon);

  try
  {
    if (!mapping.get("id", addon->m_id))
    {
      throw std::runtime_error("(id ...) field missing from addon description");
    }

    if (addon->m_id.empty())
    {
      throw std::runtime_error("Add-on id is empty");
    }

    if (addon->m_id.find_first_not_of(s_allowed_characters) != std::string::npos)
    {
      throw std::runtime_error("Add-on id contains illegal characters: " + addon->m_id);
    }

    mapping.get("version", addon->m_version);

    std::string type;
    mapping.get("type", type);
    addon->m_type = addon_string_util::addon_type_from_string(type);

    mapping.get("title", addon->m_title);
    mapping.get("author", addon->m_author);
    mapping.get("license", addon->m_license);
    mapping.get("description", addon->m_description);
    mapping.get("url", addon->m_url);
    mapping.get("md5", addon->m_md5);
    mapping.get("format", addon->m_format);
    std::optional<ReaderCollection> screenshots_reader;
    if (mapping.get("screenshots", screenshots_reader))
    {
      for (auto& obj : screenshots_reader->get_objects())
      {
        std::string url;
        auto data = obj.get_mapping();
        data.get("url", url);
        addon->m_screenshots.push_back(url);
      }
    }
    std::optional<ReaderCollection> dependencies_reader;
    if (mapping.get("dependencies", dependencies_reader))
    {
      for (auto& obj : dependencies_reader->get_objects())
      {
        std::string id;
        auto data = obj.get_mapping();
        data.get("id", id);
        addon->m_dependencies.push_back(id);
      }
    }

    return addon;
  }
  catch(const std::exception& err)
  {
    std::stringstream msg;
    msg << "Problem when parsing addoninfo: " << err.what();
    throw std::runtime_error(msg.str());
  }
}

std::unique_ptr<Addon>
Addon::parse(const std::string& fname)
{
  try
  {
    register_translation_directory(fname);
    auto doc = ReaderDocument::from_file(fname);
    auto root = doc.get_root();
    if (root.get_name() != "supertux-addoninfo")
    {
      throw std::runtime_error("File is not a supertux-addoninfo file.");
    }
    else
    {
      return parse(root.get_mapping());
    }
  }
  catch(const std::exception& err)
  {
    std::stringstream msg;
    msg << "Problem when reading addoninfo '" << fname << "': " << err.what();
    throw std::runtime_error(msg.str());
  }
}

Addon::Addon() :
  m_id(),
  m_version(0),
  m_type(),
  m_title(),
  m_author(),
  m_license(),
  m_format(0),
  m_description(),
  m_url(),
  m_md5(),
  m_screenshots(),
  m_dependencies(),
  m_install_filename(),
  m_enabled(false)
{}

std::string
Addon::get_filename() const
{
  return get_id() + ".zip";
}

bool
Addon::is_levelset() const
{
  // Determines if the add-on is a levelset.
  return m_type == WORLD || m_type == WORLDMAP || m_type == LEVELSET;
}

bool
Addon::overrides_data() const
{
  // Determines if the add-on should override game data.
  return m_type == RESOURCEPACK;
}

bool
Addon::requires_restart() const
{
  // Determines if the add-on requires a restart to function after enabled.
  return m_type == LANGUAGEPACK;
}

/* EOF */
