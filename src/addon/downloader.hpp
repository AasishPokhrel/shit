//  SuperTux
//  Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
//                2014 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_ADDON_DOWNLOADER_HPP
#define HEADER_SUPERTUX_ADDON_DOWNLOADER_HPP

#ifndef EMSCRIPTEN
#include <curl/curl.h>
#include <curl/easy.h>
#endif
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "addon/downloader_defines.hpp"

typedef int TransferId;

class TransferStatus final
{
  friend class Downloader;
  friend class TransferStatusList;

public:
  Downloader& m_downloader;
  TransferId id;
  const std::string file;
  std::vector<std::function<void (bool)> > callbacks;

  int dltotal;
  int dlnow;
  int ultotal;
  int ulnow;

  std::string error_msg;

private:
  TransferStatusList* parent_list;

public:
  TransferStatus(Downloader& downloader, TransferId id_,
                 const std::string& url);

  void abort();
  void update();

  void then(const std::function<void (bool)>& callback)
  {
    callbacks.push_back(callback);
  }

private:
  TransferStatus(const TransferStatus&) = delete;
  TransferStatus& operator=(const TransferStatus&) = delete;
};

class TransferStatusList final
{
  friend class Downloader;

private:
  std::vector<TransferStatusPtr> m_transfer_statuses;
  int m_successful_count;
  std::vector<std::function<void (bool)> > m_callbacks;

  std::string m_error_msg;

public:
  TransferStatusList();
  TransferStatusList(const std::vector<TransferStatusPtr>& list);

  void abort();
  void update();

  void push(TransferStatusPtr status);
  void push(TransferStatusListPtr statuses);

  void then(const std::function<void (bool)>& callback)
  {
    m_callbacks.push_back(callback);
  }

  int get_download_now() const;
  int get_download_total() const;
  inline const std::string& get_error() const { return m_error_msg; }

  bool is_active() const;

private:
  // Called when one of the transfers completes.
  void on_transfer_complete(TransferStatusPtr this_status, bool successful);

  void reset();

private:
  TransferStatusList(const TransferStatusList&) = delete;
  TransferStatusList& operator=(const TransferStatusList&) = delete;
};

class Transfer;

class Downloader final
{
private:
#ifndef EMSCRIPTEN
  CURLM* m_multi_handle;
#endif
  std::map<TransferId, std::unique_ptr<Transfer> > m_transfers;
  int m_next_transfer_id;

  float m_last_update_time;

public:
  Downloader();
  ~Downloader();

  /** Download \a url and return the result as string */
  std::string download(const std::string& url);

  /** Download \a url and store the result in \a filename */
  void download(const std::string& url, const std::string& filename);

  void download(const std::string& url,
                size_t (*write_func)(void* ptr, size_t size, size_t nmemb, void* userdata),
                void* userdata);

  void update();

  TransferStatusPtr request_download(const std::string& url, const std::string& filename);
  void abort(TransferId id);

#ifdef EMSCRIPTEN
  void onDownloadProgress(int id, int loaded, int total);
  void onDownloadFinished(int id);
  void onDownloadError(int id);
  void onDownloadAborted(int id);
#endif

private:
  Downloader(const Downloader&) = delete;
  Downloader& operator=(const Downloader&) = delete;
};

#endif

/* EOF */
