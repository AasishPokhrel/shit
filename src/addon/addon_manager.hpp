//  SuperTux - Add-on Manager
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

#ifndef HEADER_SUPERTUX_ADDON_ADDON_MANAGER_HPP
#define HEADER_SUPERTUX_ADDON_ADDON_MANAGER_HPP

#include <memory>
#include <string>
#include <map>
#include <vector>

#include "addon/downloader.hpp"
#include "supertux/gameconfig.hpp"
#include "util/currenton.hpp"

class Addon;
class AddonDependencyManager;

typedef std::string AddonId;

/** Checks for, installs and removes Add-ons */
class AddonManager final : public Currenton<AddonManager>
{
public:
  using AddonMap = std::map<AddonId, std::unique_ptr<Addon> >;

private:
  Downloader m_downloader;
  const std::string m_addon_directory;
  const std::string m_cache_directory;
  const std::string m_screenshots_cache_directory;
  std::string m_repository_url;
  std::vector<Config::Addon>& m_addon_config;

  AddonMap m_installed_addons;
  AddonMap m_repository_addons;

  bool m_initialized;
  bool m_has_been_updated;

  TransferStatusListPtr m_transfer_statuses;

public:
  AddonManager(const std::string& addon_directory,
               std::vector<Config::Addon>& addon_config);
  ~AddonManager() override;

  void empty_cache_directory();

  inline bool has_online_support() const { return true; }
  inline bool has_been_updated() const { return m_has_been_updated; }
  void check_online();
  TransferStatusPtr request_check_online();

  std::vector<AddonId> get_repository_addons() const;
  std::vector<AddonId> get_installed_addons() const;

  Addon& get_repository_addon(const AddonId& addon) const;
  Addon& get_installed_addon(const AddonId& addon) const;

  TransferStatusListPtr request_install_addon(const AddonId& addon_id);
  TransferStatusListPtr request_install_addon_dependencies(const AddonId& addon_id);
  void install_addon(const AddonId& addon_id);
  void uninstall_addon(const AddonId& addon_id);
  void install_addon_from_local_file(const std::string& filename);

  TransferStatusListPtr request_download_addon_screenshots(const AddonId& addon_id);
  std::vector<std::string> get_local_addon_screenshots(const AddonId& addon_id);

  void enable_addon(const AddonId& addon_id);
  void disable_addon(const AddonId& addon_id);

  bool is_old_enabled_addon(const std::unique_ptr<Addon>& addon) const;
  bool is_old_addon_enabled() const;
  void disable_old_addons();
  void mount_old_addons();
  void unmount_old_addons();
  bool is_from_old_addon(const std::string& filename) const;
  bool is_addon_installed(const std::string& id) const;

  std::vector<AddonId> get_depending_addons(const std::string& id) const;

  void update();
  void check_for_langpack_updates();

#ifdef EMSCRIPTEN
  void onDownloadProgress(int id, int loaded, int total);
  void onDownloadFinished(int id);
  void onDownloadError(int id);
  void onDownloadAborted(int id);
#endif

private:
  TransferStatusListPtr request_install_addon_dependencies(const Addon& addon);

  std::vector<std::string> scan_for_archives() const;
  void add_installed_addons();
  AddonMap parse_addon_infos(const std::string& filename) const;

  /** add \a archive, given as physfs path, to the list of installed
      archives */
  void add_installed_archive(const std::string& archive, const std::string& md5, bool user_install = false);

  /** search for an .nfo file in the top level directory that
      originates from \a archive, \a archive is a OS path */
  std::string scan_for_info(const std::string& archive) const;

private:
  AddonManager(const AddonManager&) = delete;
  AddonManager& operator=(const AddonManager&) = delete;
};

#endif

/* EOF */
