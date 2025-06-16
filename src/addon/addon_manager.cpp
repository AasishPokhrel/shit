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

#include "addon/addon_manager.hpp"

#include <physfs.h>
#include <fmt/format.h>
#include <sstream>

#include "addon/addon.hpp"
#include "addon/md5.hpp"
#include "gui/dialog.hpp"
#include "physfs/util.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/addon_menu.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/resources.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/string_util.hpp"

namespace {

static const char* ADDON_INFO_PATH = "/addons/repository.nfo";
static const char* ADDON_REPOSITORY_URL = "https://raw.githubusercontent.com/SuperTux/addons/master/index-0_6.nfo";

MD5 md5_from_file(const std::string& filename)
{
  // TODO: This does not work as expected for some files -- IFileStream seems to not always behave like an ifstream.
  //IFileStream ifs(installed_physfs_filename);
  //std::string md5 = MD5(ifs).hex_digest();

  MD5 md5;

  auto file = PHYSFS_openRead(filename.c_str());
  if (!file)
  {
    std::ostringstream out;
    out << "PHYSFS_openRead() failed: " << physfsutil::get_last_error();
    throw std::runtime_error(out.str());
  }
  else
  {
    while (true)
    {
      unsigned char buffer[1024];
      PHYSFS_sint64 len = PHYSFS_readBytes(file, buffer, sizeof(buffer));
      if (len <= 0) break;
      md5.update(buffer, static_cast<unsigned int>(len));
    }
    PHYSFS_close(file);

    return md5;
  }
}

MD5 md5_from_archive(const std::string& filename)
{
  if (physfsutil::is_directory(filename)) {
    return MD5();
  } else {
    return md5_from_file(filename);
  }
}

static Addon& get_addon(const AddonManager::AddonMap& list, const AddonId& id,
                        bool installed)
{
  auto it = list.find(id);

  if (it != list.end())
  {
    return *(it->second);
  }
  else
  {
    std::string type = installed ? "installed" : "repository";
    throw std::runtime_error("Couldn't find " + type + " addon with id: " + id);
  }
}

static std::vector<AddonId> get_addons(const AddonManager::AddonMap& list)
{
  // Use a map for storing sorted add-on titles with their respective IDs.
  std::map<std::string, AddonId> sorted_titles;
  for (const auto& [id, addon] : list)
  {
    sorted_titles.insert({addon->get_title(), id});
  }
  std::vector<AddonId> results;
  results.reserve(sorted_titles.size());
  std::transform(sorted_titles.begin(), sorted_titles.end(),
                 std::back_inserter(results),
                 [](const auto& title_and_id)
                 {
                   return title_and_id.second;
                 });
  return results;
}

static PHYSFS_EnumerateCallbackResult add_to_dictionary_path(void *data, const char *origdir, const char *fname)
{
    std::string full_path = FileSystem::join(origdir, fname);
    if (physfsutil::is_directory(full_path))
    {
        log_debug << "Adding \"" << full_path << "\" to dictionary search path" << std::endl;
        // We want translations from add-ons to have precedence.
        g_dictionary_manager->add_directory(full_path, true);
    }
    return PHYSFS_ENUM_OK;
}

static PHYSFS_EnumerateCallbackResult remove_from_dictionary_path(void *data, const char *origdir, const char *fname)
{
    std::string full_path = FileSystem::join(origdir, fname);
    if (physfsutil::is_directory(full_path))
    {
        g_dictionary_manager->remove_directory(full_path);
    }
    return PHYSFS_ENUM_OK;
}

} // namespace

AddonManager::AddonManager(const std::string& addon_directory,
                           std::vector<Config::Addon>& addon_config) :
  m_downloader(),
  m_addon_directory(addon_directory),
  m_cache_directory(FileSystem::join(m_addon_directory, "cache")),
  m_screenshots_cache_directory(FileSystem::join(m_cache_directory, "screenshots")),
  m_repository_url(ADDON_REPOSITORY_URL),
  m_addon_config(addon_config),
  m_installed_addons(),
  m_repository_addons(),
  m_initialized(false),
  m_has_been_updated(false),
  m_transfer_statuses(new TransferStatusList)
{
  if (!PHYSFS_mkdir(m_addon_directory.c_str()))
  {
    std::ostringstream msg;
    msg << "Couldn't create directory for addons '"
        << m_addon_directory << "': " << physfsutil::get_last_error();
    throw std::runtime_error(msg.str());
  }

  add_installed_addons();

  // FIXME: We should also restore the order here.
  for (auto& addon : m_addon_config)
  {
    if (addon.enabled)
    {
      try
      {
        enable_addon(addon.id);
      }
      catch(const std::exception& err)
      {
        log_warning << "Failed to enable addon '" << addon.id << "' from config: " << err.what() << std::endl;
      }
    }
  }

  if (PHYSFS_exists(ADDON_INFO_PATH))
  {
    try
    {
      m_repository_addons = parse_addon_infos(ADDON_INFO_PATH);
    }
    catch(const std::exception& err)
    {
      log_warning << "parsing repository.nfo failed: " << err.what() << std::endl;
    }
  }
  else
  {
    log_info << "repository.nfo doesn't exist, not loading" << std::endl;
  }

  if (!g_config->repository_url.empty() &&
      g_config->repository_url != m_repository_url)
  {
    m_repository_url = g_config->repository_url;
  }

  // Create the add-on cache directory, if it doesn't exist.
  if (!PHYSFS_exists(m_cache_directory.c_str()))
  {
    PHYSFS_mkdir(m_cache_directory.c_str());
  }
  else
  {
    empty_cache_directory();
  }

  m_initialized = true;
}

AddonManager::~AddonManager()
{
  // Sync enabled/disabled add-ons into the config for saving.
  m_addon_config.clear();
  for (const auto& [id, addon] : m_installed_addons)
  {
    m_addon_config.push_back({id, addon->is_enabled()});
  }

  // Delete the add-on cache directory, if it exists.
  physfsutil::remove_with_content(m_cache_directory);
}

void
AddonManager::empty_cache_directory()
{
  physfsutil::remove_content(m_cache_directory);
}

Addon&
AddonManager::get_repository_addon(const AddonId& id) const
{
  return get_addon(m_repository_addons, id, false);
}

Addon&
AddonManager::get_installed_addon(const AddonId& id) const
{
  return get_addon(m_installed_addons, id, true);
}

std::vector<AddonId>
AddonManager::get_repository_addons() const
{
  return get_addons(m_repository_addons);
}


std::vector<AddonId>
AddonManager::get_installed_addons() const
{
  return get_addons(m_installed_addons);
}

TransferStatusPtr
AddonManager::request_check_online()
{
  empty_cache_directory();

  // Since then() may be called immediately if networking is disabled,
  // hold the status in a separate variable so that `m_transfer_status = {}`
  // below doesn't cause this function to return nullptr.
  auto status = m_downloader.request_download(m_repository_url, ADDON_INFO_PATH);
  status->then(
    [this](bool success)
    {
      if (success)
      {
        m_repository_addons = parse_addon_infos(ADDON_INFO_PATH);
        m_has_been_updated = true;
      }
    });

  return status;
}

void
AddonManager::check_online()
{
  empty_cache_directory();
  m_downloader.download(m_repository_url, ADDON_INFO_PATH);
  m_repository_addons = parse_addon_infos(ADDON_INFO_PATH);
  m_has_been_updated = true;
}

TransferStatusListPtr
AddonManager::request_install_addon(const AddonId& addon_id)
{
  // Remove add-on if it already exists.
  auto it = m_installed_addons.find(addon_id);
  if (it != m_installed_addons.end())
  {
    log_debug << "Reinstalling add-on " << addon_id << std::endl;
    if (it->second->is_enabled())
    {
      disable_addon(it->first);
    }
    m_installed_addons.erase(it);
  }
  else
  {
    log_debug << "Installing add-on " << addon_id << std::endl;
  }

  auto& addon = get_repository_addon(addon_id);

  std::string install_filename = FileSystem::join(m_addon_directory, addon.get_filename());

  // Install add-on dependencies, if any.
  request_install_addon_dependencies(addon);

  // Install the add-on.
  TransferStatusPtr status = m_downloader.request_download(addon.get_url(), install_filename);
  status->then(
    [this, install_filename, addon_id](bool success)
    {
      if (success)
      {
        // Complete the add-on installation.
        Addon& repository_addon = get_repository_addon(addon_id);

        MD5 md5 = md5_from_file(install_filename);
        if (repository_addon.get_md5() != md5.hex_digest())
        {
          if (PHYSFS_delete(install_filename.c_str()) == 0)
          {
            log_warning << "PHYSFS_delete failed: " << physfsutil::get_last_error() << std::endl;
          }

          throw std::runtime_error("Downloading Add-on failed: MD5 checksums differ");
        }
        else
        {
          const char* realdir = PHYSFS_getRealDir(install_filename.c_str());
          if (!realdir)
          {
            throw std::runtime_error("PHYSFS_getRealDir failed: " + install_filename);
          }

          add_installed_archive(install_filename, md5.hex_digest());

          // Attempt to enable the add-on.
          try
          {
            enable_addon(repository_addon.get_id());
          }
          catch (const std::exception& err)
          {
            log_warning << "Enabling add-on failed: " << err.what() << std::endl;
          }
        }
      }
    });
  m_transfer_statuses->push(status);

  return m_transfer_statuses;
}

TransferStatusListPtr
AddonManager::request_install_addon_dependencies(const Addon& addon)
{
  for (const std::string& id : addon.get_dependencies())
  {
    if (is_addon_installed(id))
      continue; // Don't attempt to install add-ons that are already installed.

    try
    {
      get_repository_addon(id);
    }
    catch (...)
    {
      continue; // Don't attempt to install add-ons that are not available.
    }

    request_install_addon(id);
  }

  return m_transfer_statuses;
}

TransferStatusListPtr
AddonManager::request_install_addon_dependencies(const AddonId& addon_id)
{
  return request_install_addon_dependencies(get_repository_addon(addon_id));
}

void
AddonManager::install_addon(const AddonId& addon_id)
{
  { // remove addon if it already exists.
    auto it = m_installed_addons.find(addon_id);
    if (it != m_installed_addons.end())
    {
      log_debug << "Reinstalling add-on " << addon_id << std::endl;
      if (it->second->is_enabled())
      {
        disable_addon(it->first);
      }
      m_installed_addons.erase(it);
    }
    else
    {
      log_debug << "Installing add-on " << addon_id << std::endl;
    }
  }

  auto& repository_addon = get_repository_addon(addon_id);

  std::string install_filename = FileSystem::join(m_addon_directory, repository_addon.get_filename());

  m_downloader.download(repository_addon.get_url(), install_filename);

  MD5 md5 = md5_from_file(install_filename);
  if (repository_addon.get_md5() != md5.hex_digest())
  {
    if (PHYSFS_delete(install_filename.c_str()) == 0)
    {
      log_warning << "PHYSFS_delete failed: " << physfsutil::get_last_error() << std::endl;
    }

    throw std::runtime_error("Downloading Add-on failed: MD5 checksums differ");
  }
  else
  {
    const char* realdir = PHYSFS_getRealDir(install_filename.c_str());
    if (!realdir)
    {
      throw std::runtime_error("PHYSFS_getRealDir failed: " + install_filename);
    }
    else
    {
      add_installed_archive(install_filename, md5.hex_digest());
    }
  }
}

void
AddonManager::install_addon_from_local_file(const std::string& filename)
{
  const std::string& source_filename = FileSystem::basename(filename);
  if(!StringUtil::has_suffix(source_filename, ".zip"))
    return;

  const std::string& target_directory = FileSystem::join(PHYSFS_getRealDir(m_addon_directory.c_str()), m_addon_directory);
  const std::string& target_filename = FileSystem::join(target_directory, source_filename);
  const std::string& physfs_target_filename = FileSystem::join(m_addon_directory, source_filename);

  FileSystem::copy(filename, target_filename);
  MD5 target_md5 = md5_from_file(physfs_target_filename);
  add_installed_archive(physfs_target_filename, target_md5.hex_digest(), true);
}

void
AddonManager::uninstall_addon(const AddonId& addon_id)
{
  log_debug << "Uninstalling add-on " << addon_id << std::endl;
  auto& addon = get_installed_addon(addon_id);
  if (addon.is_enabled())
  {
    disable_addon(addon_id);
  }
  log_debug << "Deleting file \"" << addon.get_install_filename() << "\"" << std::endl;
  const auto it = m_installed_addons.find(addon.get_id());
  if (it != m_installed_addons.end())
  {
    if (PHYSFS_delete(FileSystem::join(m_addon_directory, addon.get_filename()).c_str()) == 0)
    {
      throw std::runtime_error("Error deleting addon .zip file: \"PHYSFS_delete\" failed: " + std::string(physfsutil::get_last_error()));
    }
    m_installed_addons.erase(it);
  }
  else
  {
    throw std::runtime_error("Error uninstalling add-on: Addon with id " + addon_id + " not found.");
  }
}

TransferStatusListPtr
AddonManager::request_download_addon_screenshots(const AddonId& addon_id)
{
  // Create the add-on screenshots cache directory, if it doesn't exist.
  if (!PHYSFS_exists(m_screenshots_cache_directory.c_str()))
  {
    PHYSFS_mkdir(m_screenshots_cache_directory.c_str());
  }

  const auto& screenshots = get_repository_addon(addon_id).get_screenshots();
  for (size_t i = 0; i < screenshots.size(); i++)
  {
    const std::string filename = addon_id + "_" + std::to_string(i + 1) + FileSystem::extension(screenshots[i]);
    const std::string filepath = FileSystem::join(m_screenshots_cache_directory, filename);
    if (PHYSFS_exists(filepath.c_str())) continue; // Do not re-download existing screenshots.

    TransferStatusPtr status;
    try
    {
      status = m_downloader.request_download(screenshots[i], filepath);
    }
    catch (std::exception& err)
    {
      log_warning << "Error downloading add-on screenshot from URL '" << screenshots[i]
                  << "' to file '" << filename << "': " << err.what() << std::endl;
      continue;
    }

    m_transfer_statuses->push(status);
  }
  return m_transfer_statuses;
}

std::vector<std::string>
AddonManager::get_local_addon_screenshots(const AddonId& addon_id)
{
  std::vector<std::string> screenshots;
  physfsutil::enumerate_files_alphabetical(m_screenshots_cache_directory, [&screenshots, &addon_id, this](const std::string& filename) {
    // Push any files from the cache directory, starting with the ID of the add-on.
    if (StringUtil::starts_with(filename, addon_id))
    {
      screenshots.push_back(FileSystem::join(m_screenshots_cache_directory, filename));
    }
    return false;
  });
  return screenshots;
}

void
AddonManager::enable_addon(const AddonId& addon_id)
{
  log_debug << "enabling addon " << addon_id << std::endl;
  auto& addon = get_installed_addon(addon_id);
  if (addon.is_enabled())
  {
    throw std::runtime_error("Tried enabling already enabled add-on.");
  }
  else
  {
    if (addon.get_type() == Addon::RESOURCEPACK)
    {
      for (const auto& [id, installed_addon] : m_installed_addons)
      {
        if (installed_addon->get_type() == Addon::RESOURCEPACK &&
            installed_addon->is_enabled())
        {
          throw std::runtime_error(_("Only one resource pack is allowed to be enabled at a time."));
        }
      }
    }

    std::string mountpoint;
    switch (addon.get_format()) {
      case Addon::ORIGINAL:
        mountpoint = "";
        break;
      default:
        mountpoint = "custom/" + addon_id;
        break;
    }

    log_debug << "Adding archive \"" << addon.get_install_filename() << "\" to search path" << std::endl;
    if (PHYSFS_mount(addon.get_install_filename().c_str(), mountpoint.c_str(), !addon.overrides_data()) == 0)
    {
      std::stringstream err;
      err << "Could not add " << addon.get_install_filename() << " to search path: "
          << physfsutil::get_last_error() << std::endl;
      throw std::runtime_error(err.str());
    }
    else
    {
      if (addon.get_type() == Addon::LANGUAGEPACK)
        PHYSFS_enumerate(addon.get_id().c_str(), add_to_dictionary_path, nullptr);

      if (m_initialized && addon.overrides_data())
        Resources::reload_all();

      addon.set_enabled(true);
    }
  }
}

void
AddonManager::disable_addon(const AddonId& addon_id)
{
  log_debug << "disabling addon " << addon_id << std::endl;
  auto& addon = get_installed_addon(addon_id);
  if (!addon.is_enabled())
  {
    throw std::runtime_error("Tried disabling already disabled add-on.");
  }
  else
  {
    log_debug << "Removing archive \"" << addon.get_install_filename() << "\" from search path" << std::endl;
    if (PHYSFS_unmount(addon.get_install_filename().c_str()) == 0)
    {
      std::stringstream err;
      err << "Could not remove " << addon.get_install_filename() << " from search path: "
          << physfsutil::get_last_error() << std::endl;
      throw std::runtime_error(err.str());
    }
    else
    {
      if (addon.get_type() == Addon::LANGUAGEPACK)
        PHYSFS_enumerate(addon.get_id().c_str(), remove_from_dictionary_path, nullptr);

      if (m_initialized && addon.overrides_data())
        Resources::reload_all();

      addon.set_enabled(false);
    }
  }
}

bool
AddonManager::is_old_enabled_addon(const std::unique_ptr<Addon>& addon) const
{
  return addon->get_format() == Addon::ORIGINAL &&
         addon->get_type() != Addon::LANGUAGEPACK &&
         addon->is_enabled();
}

bool
AddonManager::is_old_addon_enabled() const {
  auto it = std::find_if(m_installed_addons.begin(), m_installed_addons.end(),
                         [this](const auto& addon)
                         {
                           return is_old_enabled_addon(addon.second);
                         });

  return it != m_installed_addons.end();
}

void
AddonManager::disable_old_addons()
{
  for (auto& [id, addon] : m_installed_addons) {
    if (is_old_enabled_addon(addon)) {
      disable_addon(id);
    }
  }
}

void
AddonManager::mount_old_addons()
{
  std::string mountpoint;
  for (auto& [id, addon] : m_installed_addons) {
    if (is_old_enabled_addon(addon)) {
      if (PHYSFS_mount(addon->get_install_filename().c_str(), mountpoint.c_str(), !addon->overrides_data()) == 0)
      {
        log_warning << "Could not add " << addon->get_install_filename() << " to search path: "
                    << physfsutil::get_last_error() << std::endl;
      }
    }
  }
}

void
AddonManager::unmount_old_addons()
{
  for (auto& [id, addon] : m_installed_addons) {
    if (is_old_enabled_addon(addon)) {
      if (PHYSFS_unmount(addon->get_install_filename().c_str()) == 0)
      {
        log_warning << "Could not remove " << addon->get_install_filename() << " from search path: "
                    << physfsutil::get_last_error() << std::endl;
      }
    }
  }
}

bool
AddonManager::is_from_old_addon(const std::string& filename) const
{
  std::string real_path = PHYSFS_getRealDir(filename.c_str());
  for (auto& [id, addon] : m_installed_addons) {
    if (is_old_enabled_addon(addon) &&
        addon->get_install_filename() == real_path) {
      return true;
    }
  }
  return false;
}

bool
AddonManager::is_addon_installed(const std::string& id) const
{
  const auto installed_addons = get_installed_addons();
  return std::any_of(installed_addons.begin(), installed_addons.end(),
    [id] (const auto& installed_addon) {
      return installed_addon == id;
    });
}

std::vector<AddonId>
AddonManager::get_depending_addons(const std::string& id) const
{
  std::vector<AddonId> addons;
  for (auto& [addon_id, addon] : m_installed_addons)
  {
    const auto& dependencies = addon->get_dependencies();
    if (std::find(dependencies.begin(), dependencies.end(), addon_id) != dependencies.end())
      addons.push_back(addon_id);
  }
  return addons;
}

std::vector<std::string>
AddonManager::scan_for_archives() const
{
  std::vector<std::string> archives;

  // Search for archives and add them to the search path.
  physfsutil::enumerate_files(m_addon_directory, [this, &archives](const std::string& filename) {
    const std::string fullpath = FileSystem::join(m_addon_directory, filename);
    if (physfsutil::is_directory(fullpath))
    {
      // ignore dot files (e.g. '.git/'), as well as the addon cache directory.
      if (filename[0] != '.' && fullpath != m_cache_directory) {
        archives.push_back(fullpath);
      }
    }
    else
    {
      if (StringUtil::has_suffix(StringUtil::tolower(filename), ".zip")) {
        if (PHYSFS_exists(fullpath.c_str())) {
          archives.push_back(fullpath);
        }
      }
    }
    return false;
  });

  return archives;
}

std::string
AddonManager::scan_for_info(const std::string& archive_os_path) const
{
  std::string nfoFilename = "";
  physfsutil::enumerate_files("/", [archive_os_path, &nfoFilename](const std::string& file) {
    if (StringUtil::has_suffix(file, ".nfo"))
    {
      std::string nfo_filename = FileSystem::join("/", file);

      // Make sure it's in the current archive_os_path.
      const char* realdir = PHYSFS_getRealDir(nfo_filename.c_str());
      if (!realdir)
      {
        log_warning << "PHYSFS_getRealDir() failed for " << nfo_filename << ": " << physfsutil::get_last_error() << std::endl;
      }
      else
      {
        if (realdir == archive_os_path)
        {
          nfoFilename = nfo_filename;
          return true;
        }
      }
    }
    return false;
  });

  return nfoFilename;
}

void
AddonManager::add_installed_archive(const std::string& archive, const std::string& md5, bool user_install)
{
  const char* realdir = PHYSFS_getRealDir(archive.c_str());
  if (!realdir)
  {
    log_warning << "PHYSFS_getRealDir() failed for " << archive << ": "
                << physfsutil::get_last_error() << std::endl;
  }
  else
  {
    bool has_error = false;
    std::string os_path = FileSystem::join(realdir, archive);

    PHYSFS_mount(os_path.c_str(), nullptr, 1);

    std::string nfo_filename = scan_for_info(os_path);

    if (nfo_filename.empty())
    {
      log_warning << "Couldn't find .nfo file for " << os_path << std::endl;
      has_error = true;
    }
    else
    {
      try
      {
        std::unique_ptr<Addon> addon = Addon::parse(nfo_filename);
        addon->set_install_filename(os_path, md5);
        const auto& addon_id = addon->get_id();

        try
        {
          get_installed_addon(addon_id);
          if(user_install)
          {
            Dialog::show_message(fmt::format(_("Add-on {} by {} is already installed."),
                                             addon->get_title(), addon->get_author()));
          }
        }
        catch(...)
        {
          // Save add-on title and author on stack before std::move.
          const std::string addon_title = addon->get_title();
          const std::string addon_author = addon->get_author();
          m_installed_addons[addon_id] = std::move(addon);
          if(user_install)
          {
            try
            {
              enable_addon(addon_id);
            }
            catch(const std::exception& err)
            {
              log_warning << "Failed to enable add-on archive '" << addon_id << "': " << err.what() << std::endl;
            }
            Dialog::show_message(fmt::format(_("Add-on {} by {} successfully installed."),
                                             addon_title, addon_author));
            // If currently opened menu is add-ons menu refresh it.
            AddonMenu* addon_menu = dynamic_cast<AddonMenu*>(MenuManager::instance().current_menu());
            if (addon_menu)
              addon_menu->refresh();
          }
        }
      }
      catch (const std::runtime_error& e)
      {
        log_warning << "Could not load add-on info for " << archive << ": " << e.what() << std::endl;
        has_error = true;
      }
    }

    if(!user_install || has_error)
    {
      PHYSFS_unmount(os_path.c_str());
    }
  }
}

void
AddonManager::add_installed_addons()
{
  auto archives = scan_for_archives();

  for (const auto& archive : archives)
  {
    MD5 md5 = md5_from_archive(archive);
    add_installed_archive(archive, md5.hex_digest());
  }
}

AddonManager::AddonMap
AddonManager::parse_addon_infos(const std::string& filename) const
{
  AddonMap m_addons;

  try
  {
    register_translation_directory(filename);
    auto doc = ReaderDocument::from_file(filename);
    auto root = doc.get_root();
    if (root.get_name() != "supertux-addons")
    {
      throw std::runtime_error("Downloaded file is not an Add-on list");
    }
    else
    {
      auto addon_collection = root.get_collection();
      for (auto const& addon_node : addon_collection.get_objects())
      {
        if (addon_node.get_name() != "supertux-addoninfo")
        {
          log_warning << "Unknown token '" << addon_node.get_name() << "' in Add-on list" << std::endl;
        }
        else
        {
          try
          {
            std::unique_ptr<Addon> addon = Addon::parse(addon_node.get_mapping());
            m_addons[addon->get_id()] = std::move(addon);
          }
          catch(const std::exception& e)
          {
            log_warning << "Problem when reading Add-on entry: " << e.what() << std::endl;
          }
        }
      }

      return m_addons;
    }
  }
  catch(const std::exception& e)
  {
    std::stringstream msg;
    msg << "Problem when reading Add-on list: " << e.what();
    throw std::runtime_error(msg.str());
  }
}

void
AddonManager::update()
{
  m_downloader.update();
}

void
AddonManager::check_for_langpack_updates()
{
  const std::string& language = g_dictionary_manager->get_language().get_language();
  if (language == "en")
    return;

  try
  {
    check_online();
    try
    {
      const std::string& addon_id = "language-pack";
      log_debug << "Looking for language add-on with ID " << addon_id << "..." << std::endl;
      Addon& langpack = get_repository_addon(addon_id);

      try
      {
        auto& installed_langpack = get_installed_addon(addon_id);
        if (installed_langpack.get_md5() == langpack.get_md5() ||
            installed_langpack.get_version() > langpack.get_version())
        {
          log_debug << "Language add-on " << addon_id << " is already the latest version." << std::endl;
          return;
        }

        // Langpack update available. Let's install it!
        install_addon(addon_id);
        try
        {
          enable_addon(addon_id);
        }
        catch(const std::exception& err)
        {
          log_warning << "Failed to enable language pack '" << addon_id << "' after update: " << err.what() << std::endl;
        }
      }
      catch(const std::exception&)
      {
        log_debug << "Language addon " << addon_id << " is not installed. Installing..." << std::endl;
        install_addon(addon_id);
        try
        {
          enable_addon(addon_id);
        }
        catch(const std::exception& err)
        {
          log_warning << "Failed to enable language pack '" << addon_id << "' after install: " << err.what() << std::endl;
        }
      }
    }
    catch(std::exception&)
    {
      log_debug << "Language add-on for current locale not found." << std::endl;
    }
  }
  catch(...)
  {
    // If anything fails here, just silently ignore.
  }
}

#ifdef EMSCRIPTEN
void
AddonManager::onDownloadProgress(int id, int loaded, int total)
{
  m_downloader.onDownloadProgress(id, loaded, total);
}

void
AddonManager::onDownloadFinished(int id)
{
  m_downloader.onDownloadFinished(id);
}

void
AddonManager::onDownloadError(int id)
{
  m_downloader.onDownloadError(id);
}

void
AddonManager::onDownloadAborted(int id)
{
  m_downloader.onDownloadAborted(id);
}
#endif

/* EOF */
