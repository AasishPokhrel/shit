//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//                2022-2023 Vankata453
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_DOWNLOAD_DIALOG_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_DOWNLOAD_DIALOG_HPP

#include "gui/dialog.hpp"

#include "addon/downloader_defines.hpp"

class DownloadDialog final : public Dialog
{
private:
  TransferStatusListPtr m_status;
  std::string m_title;
  bool m_auto_close;
  bool m_error_msg;

  int m_download_total;
  bool m_complete;

public:
  DownloadDialog(TransferStatusPtr status, bool auto_close = false,
                 bool passive = false, bool no_error_msg = false);
  DownloadDialog(TransferStatusListPtr statuses, bool auto_close = false,
                 bool passive = false, bool no_error_msg = false);

  void update() override;

  inline void set_title(const std::string& title) { m_title = title; }

private:
  void update_text();

  void on_abort();
  void on_download_complete();

private:
  DownloadDialog(const DownloadDialog&) = delete;
  DownloadDialog& operator=(const DownloadDialog&) = delete;
};

#endif

/* EOF */
