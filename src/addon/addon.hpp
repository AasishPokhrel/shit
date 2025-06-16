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

#ifndef HEADER_SUPERTUX_ADDON_ADDON_HPP
#define HEADER_SUPERTUX_ADDON_ADDON_HPP

#include <memory>
#include <vector>
#include <string>

class ReaderMapping;

class Addon final
{
public:
  static std::unique_ptr<Addon> parse(const ReaderMapping& mapping);
  static std::unique_ptr<Addon> parse(const std::string& fname);

  enum Type {
    WORLD,
    WORLDMAP,
    LEVELSET,
    LANGUAGEPACK,
    RESOURCEPACK,
    WEAKRESOURCEPACK,
    ADDON
  };

  enum Format {
    ORIGINAL = 0,
    WITH_MOUNTPOINT = 1
  };

private:
  // fields provided by the addon.zip itself
  std::string m_id;
  int m_version;
  Type m_type;
  std::string m_title;
  std::string m_author;
  std::string m_license;
  int m_format;

  // additional fields provided for addons from an addon repository
  std::string m_description;
  std::string m_url;
  std::string m_md5;
  std::vector<std::string> m_screenshots;
  std::vector<std::string> m_dependencies;

  // fields filled by the AddonManager
  std::string m_install_filename;
  bool m_enabled;

private:
  Addon();

public:
  inline const std::string& get_id() const { return m_id; }
  inline int get_version() const { return m_version; }
  inline int get_format() const { return m_format; }

  inline Type get_type() const { return m_type; }
  inline const std::string& get_title() const { return m_title; }
  inline const std::string& get_author() const { return m_author; }
  inline const std::string& get_license() const { return m_license; }

  inline const std::string& get_description() const { return m_description; }
  inline const std::string& get_url() const { return m_url; }
  inline const std::string& get_md5() const { return m_md5; }
  inline const std::vector<std::string>& get_screenshots() const { return m_screenshots; }
  inline const std::vector<std::string>& get_dependencies() const { return m_dependencies; }

  std::string get_filename() const;
  inline const std::string& get_install_filename() const { return m_install_filename; }

  inline bool is_installed() const { return !m_install_filename.empty(); }
  inline bool is_enabled() const { return m_enabled; }
  inline bool is_visible() const { return true; }

  bool is_levelset() const;
  bool overrides_data() const;
  bool requires_restart() const;

  inline void set_install_filename(const std::string& absolute_filename, const std::string& md5)
  {
    m_install_filename = absolute_filename;
    m_md5 = md5;
  }
  inline void set_enabled(bool v) { m_enabled = v; }

private:
  Addon(const Addon&) = delete;
  Addon& operator=(const Addon&) = delete;
};

namespace addon_string_util {
  Addon::Type addon_type_from_string(const std::string& type);
  std::string addon_type_to_translated_string(Addon::Type type);
  std::string generate_menu_item_text(const Addon& addon);
  std::string get_addon_plural_form(size_t count);
}

#endif

/* EOF */
