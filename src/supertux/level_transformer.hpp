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

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVEL_TRANSFORMER_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVEL_TRANSFORMER_HPP

class Level;
class Sector;

/**
 * This class is an abstract interface for algorithms that transform levels in
 * some way before they are played.
 */
class LevelTransformer
{
public:
  virtual ~LevelTransformer();

  /** transform a complete Level, the standard implementation just calls
   * transformSector on each sector in the level.
   */
  virtual void transform(Level& level);

  virtual void transform_sector(Sector& sector) = 0;
};

#endif

/* EOF */
