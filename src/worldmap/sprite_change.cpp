//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "worldmap/sprite_change.hpp"

#include "util/reader_mapping.hpp"
#include "worldmap/worldmap_sector.hpp"

namespace worldmap {

SpriteChange::SpriteChange(const ReaderMapping& mapping) :
  WorldMapObject(mapping, "images/engine/editor/spritechange.png"),
  m_change_on_touch(false),
  m_stay_action(),
  m_stay_group(),
  m_in_stay_action(false)
{
  mapping.get("change-on-touch", m_change_on_touch);
  mapping.get("stay-action", m_stay_action);
  mapping.get("initial-stay-action", m_in_stay_action);
  mapping.get("stay-group", m_stay_group);
}

SpriteChange::~SpriteChange()
{
}

void
SpriteChange::draw_worldmap(DrawingContext& context)
{
  if (m_in_stay_action && !m_stay_action.empty())
  {
    m_sprite->set_action(m_stay_action);
    WorldMapObject::draw_worldmap(context);
  }
}

bool
SpriteChange::show_stay_action() const
{
  return m_in_stay_action;
}

void
SpriteChange::set_stay_action()
{
  m_in_stay_action = true;
}

void
SpriteChange::clear_stay_action(bool propagate)
{
  m_in_stay_action = false;

  // if we are in a stay_group, also clear all stay actions in this group
  if (!m_stay_group.empty() && propagate)
  {
    for (SpriteChange& sc : WorldMapSector::current()->get_objects_by_type<SpriteChange>())
    {
      if (sc.m_stay_group != m_stay_group) continue;
      sc.m_in_stay_action = false;
    }
  }
}

SpritePtr
SpriteChange::clone_sprite() const
{
  return m_sprite->clone();
}

ObjectSettings
SpriteChange::get_settings()
{
  ObjectSettings result = WorldMapObject::get_settings();

  result.add_text(_("Stay action"), &m_stay_action, "stay-action");
  result.add_bool(_("Initial stay action"), &m_in_stay_action, "initial-stay-action");
  result.add_text(_("Stay group"), &m_stay_group, "stay-group");
  result.add_bool(_("Change on touch"), &m_change_on_touch, "change-on-touch");

  result.reorder({"change-on-touch", "initial-stay-action", "stay-group", "sprite", "x", "y"});

  return result;
}

} // namespace worldmap

/* EOF */
