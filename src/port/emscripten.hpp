//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

// Export functions for emscripten
// If you add functions here, make sure to make CMakeLists.txt export them!
#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/html5.h>

#include "addon/addon_manager.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/video_system.hpp"

extern "C" {

void set_resolution(int w, int h);
void save_config();
void init_emscripten();
void onDownloadProgress(int id, int loaded, int total);
void onDownloadFinished(int id);
void onDownloadError(int id);
void onDownloadAborted(int id);
const char* getExceptionMessage(intptr_t address);

EMSCRIPTEN_KEEPALIVE // This is probably not useful, I just want ppl to know it exists
void
set_resolution(int w, int h)
{
  VideoSystem::current()->on_resize(w, h);
  MenuManager::instance().on_window_resize();
}

EMSCRIPTEN_KEEPALIVE // Same as above
void
save_config()
{
  g_config->save();
}

void
onDownloadProgress(int id, int loaded, int total)
{
  AddonManager::current()->onDownloadProgress(id, loaded, total);
}

void
onDownloadFinished(int id)
{
  AddonManager::current()->onDownloadFinished(id);
}

void
onDownloadError(int id)
{
  AddonManager::current()->onDownloadError(id);
}

void
onDownloadAborted(int id)
{
  AddonManager::current()->onDownloadAborted(id);
}

const char*
getExceptionMessage(intptr_t address)
{
  return reinterpret_cast<std::exception*>(address)->what();
}

} // extern "C"

void
init_emscripten()
{
  EM_ASM({
    if (window.supertux_onready)
      window.supertux_onready();
  }, 0); // EM_ASM is a variadic macro and Clang requires at least 1 value for the variadic argument
}

#endif
