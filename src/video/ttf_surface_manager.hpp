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

#ifndef HEADER_SUPERTUX_VIDEO_TTF_SURFACE_MANAGER_HPP
#define HEADER_SUPERTUX_VIDEO_TTF_SURFACE_MANAGER_HPP

#include <tuple>
#include <map>
#include <string>
#include <iosfwd>

#include "util/currenton.hpp"
#include "video/color.hpp"
#include "video/surface_ptr.hpp"
#include "video/ttf_surface.hpp"

class TTFFont;

class TTFSurfaceManager final : public Currenton<TTFSurfaceManager>
{
public:
  TTFSurfaceManager();

  TTFSurfacePtr create_surface(const TTFFont& font, const std::string& text);

  // Returns -1 if there is no cached text surface
  int get_cached_surface_width(const TTFFont& font, const std::string& text);

  void clear_cache();

  void print_debug_info(std::ostream& out);

private:
  void cache_cleanup_step();

private:
  struct CacheEntry
  {
    CacheEntry() : ttf_surface(), last_access() {}
    CacheEntry(const TTFSurfacePtr& s);

    TTFSurfacePtr ttf_surface;
    float last_access;
  };

private:
  using Key = std::tuple<void*, std::string>;
  std::map<Key, CacheEntry> m_cache;

  std::map<Key, CacheEntry>::iterator m_cache_iter;

private:
  TTFSurfaceManager(const TTFSurfaceManager&) = delete;
  TTFSurfaceManager& operator=(const TTFSurfaceManager&) = delete;
};

#endif

/* EOF */
