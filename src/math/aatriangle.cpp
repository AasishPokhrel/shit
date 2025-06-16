//  $Id$
//
//  SuperTux
//  Copyright (C) 2009 Matt McCutchen
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "aatriangle.hpp"

int AATriangle::vertical_flip(int dir) {
  int direction = dir & AATriangle::DIRECTION_MASK;
  direction = 3 - direction;
  int deform = dir & AATriangle::DEFORM_MASK;
  switch (deform) {
    case AATriangle::DEFORM_BOTTOM:
      deform = AATriangle::DEFORM_TOP;
      break;
    case AATriangle::DEFORM_TOP:
      deform = AATriangle::DEFORM_BOTTOM;
      break;
    default:
      // Unchanged.
      break;
  }
  return (direction | deform);
}

/* EOF */
