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

#include "supertux/menu/download_dialog.hpp"

#include <sstream>
#include <fmt/format.h>

#include "addon/downloader.hpp"

DownloadDialog::DownloadDialog(TransferStatusPtr status, bool auto_close,
                               bool passive, bool no_error_msg) :
  DownloadDialog(TransferStatusListPtr(new TransferStatusList({ status })),
                 auto_close, passive, no_error_msg)
{
}

DownloadDialog::DownloadDialog(TransferStatusListPtr statuses, bool auto_close,
                               bool passive, bool no_error_msg) :
  Dialog(passive),
  m_status(statuses),
  m_title(),
  m_auto_close(auto_close),
  m_error_msg(!no_error_msg),
  m_download_total(0),
  m_complete(false)
{
  add_default_button(_("Abort Download"), [this]() {
    on_abort();
  });

  update_text();

  m_status->then(
    [this](bool success)
    {
      if (success)
      {
        on_download_complete();
      }
      else
      {
        if (m_error_msg)
        {
          Dialog::show_message(fmt::format(fmt::runtime(_("Error:\n{}")), m_status->get_error()));
        }
        else
        {
          MenuManager::instance().set_dialog({});
        }
      }
    });
}

void
DownloadDialog::update()
{
  m_status->update();

  update_text();
}

void
DownloadDialog::update_text()
{
  std::ostringstream out;
  out << m_title << "\n";

  int dltotal = m_complete ? m_download_total : m_status->get_download_total();
  if (dltotal == 0)
  {
    out << "---\n---";
  }
  else
  {
    int dlnow = m_complete ? m_download_total : m_status->get_download_now();
    int percent = 100 * dlnow / dltotal;
    out << dlnow / 1000 << "/"
        << dltotal / 1000 << " kB\n" << percent << "%";
  }

  set_text(out.str());
}

void
DownloadDialog::on_abort()
{
  m_status->abort();
}

void
DownloadDialog::on_download_complete()
{
  if (m_auto_close)
  {
    MenuManager::instance().set_dialog({});
    return;
  }

  m_download_total = m_status->get_download_total();
  m_complete = true;

  clear_buttons();
  add_button(_("Close"), []() {
    MenuManager::instance().set_dialog({});
  });
}

/* EOF */
