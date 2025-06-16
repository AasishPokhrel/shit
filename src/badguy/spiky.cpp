//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "badguy/spiky.hpp"

Spiky::Spiky(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/spiky/spiky.sprite", "left", "right")
{
  walk_speed = 80;
  set_ledge_behavior(LedgeBehavior::NORMAL);
}

bool
Spiky::is_freezable() const
{
  return true;
}

bool
Spiky::is_flammable() const
{
  return true;
}

/* EOF */
