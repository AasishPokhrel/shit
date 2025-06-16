//  SuperTux
//  Copyright (C) 2023 MatusGuy
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

#include "granito_giant.hpp"

GranitoGiant::GranitoGiant(const ReaderMapping& reader):
  BadGuy(reader, "images/creatures/granito/giant/granito_giant.sprite", LAYER_OBJECTS - 5)
{
  parse_type(reader);

  m_countMe = false;
}

HitResponse
GranitoGiant::collision_player(Player &player, const CollisionHit &hit)
{
  return FORCE_MOVE;
}

void
GranitoGiant::initialize()
{
  switch (m_type)
  {
    case AWAKE:
      set_action("idle", m_dir);
      break;
    case SLEEP:
      set_action("sleep", m_dir);
      break;
    case CORRUPTED_A:
      set_action("husk1", m_dir);
      break;
    case CORRUPTED_B:
      set_action("husk2", m_dir);
      break;
    case CORRUPTED_C:
      set_action("husk3", m_dir);
      break;
    default:
      break;
  }

  set_colgroup_active(COLGROUP_MOVING_STATIC);
  m_col.set_unisolid(true);
}

void
GranitoGiant::kill_fall()
{
  return;
}

GameObjectTypes
GranitoGiant::get_types() const
{
  return {
    { "awake", _("Awake") },
    { "sleep", _("Sleeping") },
    { "corrupted-a", _("Corrupted A") },
    { "corrupted-b", _("Corrupted B") },
    { "corrupted-c", _("Corrupted C") },
  };
}

std::string
GranitoGiant::get_default_sprite_name() const
{
  if (m_type < CORRUPTED_A) // Not corrupted
    return "images/creatures/granito/giant/granito_giant.sprite";
  else
    return "images/creatures/granito/corrupted/giant/corrupted_giant.sprite";
}

void
GranitoGiant::after_editor_set()
{
  BadGuy::after_editor_set();
  initialize();
}

/* EOF */
