//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/ttf_surface_manager.hpp"

#include <SDL_ttf.h>
#include <numeric>
#include <sstream>
#include <iostream>

#include "supertux/globals.hpp"
#include "video/sdl_surface_ptr.hpp"
#include "video/surface.hpp"
#include "video/ttf_font.hpp"
#include "video/ttf_surface.hpp"
#include "video/video_system.hpp"

TTFSurfaceManager::CacheEntry::CacheEntry(const TTFSurfacePtr& s) :
  ttf_surface(s),
  last_access(g_game_time)
{
}

TTFSurfaceManager::TTFSurfaceManager() :
  m_cache(),
  m_cache_iter(m_cache.end())
{
}

TTFSurfacePtr
TTFSurfaceManager::create_surface(const TTFFont& font, const std::string& text)
{
  auto key = Key(font.get_ttf_font(), text);
  auto it = m_cache.find(key);
  if (it != m_cache.end())
  {
    auto& entry = m_cache[key];
    entry.last_access = g_game_time;
    return entry.ttf_surface;
  }
  else
  {

#if 0
    // Font debug output should go to 'std::cerr', not any of the
    // log_* functions, as those are mirrored on the console which
    // in turn will lead to the creation of more TTFSurface's and
    // screw up the results.
    print_debug_info(get_logging_instance(false));
#endif

    cache_cleanup_step();

    TTFSurfacePtr ttf_surface = TTFSurface::create(font, text);
    m_cache[key] = ttf_surface;
    return ttf_surface;
  }
}

int
TTFSurfaceManager::get_cached_surface_width(const TTFFont& font,
  const std::string& text)
{
  auto key = Key(font.get_ttf_font(), text);
  auto it = m_cache.find(key);
  if (it == m_cache.end())
    return -1;
  auto& entry = m_cache[key];
  entry.last_access = g_game_time;
  return entry.ttf_surface->get_width();
}

void
TTFSurfaceManager::clear_cache()
{
  m_cache.clear();
  m_cache_iter = m_cache.begin();
}

void
TTFSurfaceManager::cache_cleanup_step()
{
  if (m_cache.empty())
    return;

  if (m_cache_iter == m_cache.end())
  {
    m_cache_iter = m_cache.begin();
  }

  while (g_game_time - m_cache_iter->second.last_access > 10.0f)
  {
    m_cache_iter = m_cache.erase(m_cache_iter);
    if (m_cache_iter == m_cache.end())
    {
      return;
    }
  }

  ++m_cache_iter;
}

void
TTFSurfaceManager::print_debug_info(std::ostream& out)
{
  int cache_bytes = std::accumulate(m_cache.begin(), m_cache.end(), 0, [](int accumulator, const std::pair<Key, CacheEntry>& entry) {
    return accumulator + entry.second.ttf_surface->get_width() * entry.second.ttf_surface->get_height() * 4;
  });
  out << "TTFSurfaceManager.cache_size: " << m_cache.size() << "  " << cache_bytes / 1000 << "KB" << std::endl;
}

/* EOF */
