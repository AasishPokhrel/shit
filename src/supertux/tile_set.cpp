//  SuperTux
//  Copyright (C) 2008 Matthias Braun <matze@braunis.de>
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

#include "supertux/tile_set.hpp"

#include "editor/editor.hpp"
#include "supertux/autotile_parser.hpp"
#include "supertux/resources.hpp"
#include "supertux/tile.hpp"
#include "supertux/tile_set_parser.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

Tilegroup::Tilegroup() :
  developers_group(),
  name(),
  tiles()
{
}

std::unique_ptr<TileSet>
TileSet::from_file(const std::string& filename)
{
  auto tileset = std::make_unique<TileSet>(filename);

  TileSetParser parser(*tileset, filename);
  parser.parse();

  tileset->print_debug_info();

  return tileset;
}

TileSet::TileSet(const std::string& filename) :
  m_filename(filename),
  m_autotilesets(),
  m_thunderstorm_tiles(),
  m_tiles(1),
  m_tilegroups()
{
  m_tiles[0] = std::make_unique<Tile>();
}

void
TileSet::reload()
{
  m_autotilesets.clear();
  m_thunderstorm_tiles.clear();
  m_tiles.resize(1); // Preserve only the initial tile with an ID of 0
  m_tilegroups.clear();

  TileSetParser parser(*this, m_filename);
  parser.parse();
}

void
TileSet::add_tile(int id, std::unique_ptr<Tile> tile)
{
  if (id >= static_cast<int>(m_tiles.size())) {
    m_tiles.resize(id + 1);
  }

  if (m_tiles[id]) {
    log_warning << "Tile with ID " << id << " redefined" << std::endl;
  } else {
    m_tiles[id] = std::move(tile);
  }
}

const Tile&
TileSet::get(const uint32_t id) const
{
  if (id >= m_tiles.size()) {
//    log_warning << "Invalid tile: " << id << std::endl;
    return *m_tiles[0];
  } else {
    assert(id < m_tiles.size());
    Tile* tile = m_tiles[id].get();
    if (tile) {
      return *tile;
    } else {
//      log_warning << "Invalid tile: " << id << std::endl;
      return *m_tiles[0];
    }
  }
}

std::vector<AutotileSet*>
TileSet::get_autotilesets_from_tile(uint32_t tile_id) const
{
  if (tile_id == 0)
  {
    return {};
  }

  std::vector<AutotileSet*> autotilesets;
  for (auto& ats : m_autotilesets)
  {
    if (ats->is_member(tile_id))
      autotilesets.push_back(ats.get());
  }
  return autotilesets;
}

bool
TileSet::has_mutual_autotileset(uint32_t lhs, uint32_t rhs) const
{
  if (lhs == rhs)
    return true;

  for (const auto& autotileset : m_autotilesets)
  {
    if (autotileset->is_member(lhs) && autotileset->is_member(rhs))
      return true;
  }
  return false;
}

void
TileSet::add_unassigned_tilegroup()
{
  Tilegroup unassigned_group;

  unassigned_group.name = _("Others");
  unassigned_group.developers_group = true;

  for (auto tile = 0; tile < static_cast<int>(m_tiles.size()); tile++)
  {
    bool found = false;
    for (const auto& group : m_tilegroups)
    {
      found = std::any_of(group.tiles.begin(), group.tiles.end(),
        [tile](const int& tile_in_group) {
          return tile_in_group == tile;
        });
      if(found)
      {
        break;
      }
    }

    // Weed out all the non-deprecated tiles that have an ID
    // but no image (mostly tiles that act as
    // spacing between other tiles).
    Tile* tile_object = m_tiles[tile].get();
    if (found == false && tile_object && !tile_object->is_deprecated())
    {
      unassigned_group.tiles.push_back(tile);
    }
  }

  if (!unassigned_group.tiles.empty())
  {
    m_tilegroups.push_back(unassigned_group);
  }
}

void
TileSet::remove_deprecated_tiles()
{
  for (Tilegroup& tilegroup : m_tilegroups)
  {
    for (int& tile : tilegroup.tiles)
    {
      if (get(tile).is_deprecated())
      {
        log_warning << "Deprecated tile " << tile << " in tilegroup \""
                    << tilegroup.name << "\", replacing with 0." << std::endl;
        tile = 0;
      }
    }
  }
}

void
TileSet::add_tilegroup(const Tilegroup& tilegroup)
{
  m_tilegroups.push_back(tilegroup);
}

void
TileSet::print_debug_info()
{
  if (false)
  { // enable this if you want to see a list of free tiles
    log_info << "Last Tile ID is " << m_tiles.size()-1 << std::endl;
    int last = -1;
    for (int i = 0; i < int(m_tiles.size()); ++i)
    {
      if (m_tiles[i] == nullptr && last == -1)
      {
        last = i;
      }
      else if (m_tiles[i] && last != -1)
      {
        log_info << "Free Tile IDs (" << i - last << "): " << last << " - " << i-1 << std::endl;
        last = -1;
      }
    }
  }
}

/* EOF */
