//  SuperTux
//  Copyright (C) 2004 Ingo Ruhnke <grumbel@gmail.com>
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

#include "worldmap/special_tile.hpp"

#include "util/reader_mapping.hpp"

namespace worldmap {

SpecialTile::SpecialTile(const ReaderMapping& mapping) :
  WorldMapObject(mapping, "images/engine/editor/specialtile.png"),
  m_map_message(),
  m_passive_message(false),
  m_script(),
  m_invisible(false),
  m_apply_direction(),
  m_apply_action_north(true),
  m_apply_action_east(true),
  m_apply_action_south(true),
  m_apply_action_west(true)
{
  mapping.get("invisible-tile", m_invisible);

  if (in_worldmap() && !has_found_sprite()) // In worldmap and no valid sprite is specified, be invisible
    m_invisible = true;

  mapping.get("map-message", m_map_message);
  mapping.get("passive-message", m_passive_message);
  mapping.get("script", m_script);

  mapping.get("apply-to-direction", m_apply_direction);
  if (!m_apply_direction.empty())
  {
    m_apply_action_north = m_apply_direction.find("north") != std::string::npos;
    m_apply_action_south = m_apply_direction.find("south") != std::string::npos;
    m_apply_action_east = m_apply_direction.find("east") != std::string::npos;
    m_apply_action_west = m_apply_direction.find("west") != std::string::npos;
  }
}

SpecialTile::~SpecialTile()
{
}

void
SpecialTile::draw_worldmap(DrawingContext& context)
{
  if (m_invisible)
    return;

  WorldMapObject::draw_worldmap(context);
}

ObjectSettings
SpecialTile::get_settings()
{
  ObjectSettings result = WorldMapObject::get_settings();

  result.add_translatable_text(_("Message"), &m_map_message, "map-message");
  result.add_bool(_("Show message"), &m_passive_message, "passive-message", false);
  result.add_script(_("Script"), &m_script, "script");
  result.add_bool(_("Invisible"), &m_invisible, "invisible-tile", false);
  result.add_text(_("Direction"), &m_apply_direction, "apply-to-direction", "north-east-south-west");

  result.reorder({"map-message", "invisible-tile", "script", "passive-message", "apply-to-direction", "sprite", "x", "y"});

  return result;
}

}

/* EOF */
