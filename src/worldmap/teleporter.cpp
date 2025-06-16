//  SuperTux - Teleporter Worldmap Tile
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "worldmap/teleporter.hpp"

#include "util/reader_mapping.hpp"

namespace worldmap {

Teleporter::Teleporter(const ReaderMapping& mapping) :
  WorldMapObject(mapping, "images/worldmap/common/teleporterdot.sprite"),
  m_worldmap(),
  m_sector(),
  m_spawnpoint(),
  m_automatic(false),
  m_message()
{
  if (in_worldmap() && !has_found_sprite()) // In worldmap and no valid sprite is specified, remove it
    m_sprite.reset();

  mapping.get("worldmap", m_worldmap);
  mapping.get("sector", m_sector);
  mapping.get("spawnpoint", m_spawnpoint);
  mapping.get("automatic", m_automatic);
  mapping.get("message", m_message);
}

ObjectSettings
Teleporter::get_settings()
{
  ObjectSettings result = WorldMapObject::get_settings();

  result.add_text(_("Sector"), &m_sector, "sector");
  result.add_text(_("Spawnpoint"), &m_spawnpoint, "spawnpoint");
  result.add_translatable_text(_("Message"), &m_message, "message");
  result.add_bool(_("Automatic"), &m_automatic, "automatic", false);
  result.add_worldmap(_("Target worldmap"), &m_worldmap, "worldmap");

  result.reorder({"sector", "spawnpoint", "automatic", "message", "sprite", "x", "y"});

  return result;
}

} // namespace worldmap

/* EOF */
