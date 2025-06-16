//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_TILE_HPP
#define HEADER_SUPERTUX_SUPERTUX_TILE_HPP

#include <vector>
#include <stdint.h>

#include "math/rectf.hpp"
#include "video/color.hpp"
#include "video/surface_ptr.hpp"

class Canvas;
class DrawingContext;

class Tile final
{
public:
  static bool draw_editor_images;

public:
  /** bitset for tile attributes */
  enum {
    /** solid tile that is indestructible by Tux */
    SOLID     = 0x0001,
    /** uni-directional solid tile */
    UNISOLID  = 0x0002,
    /** a brick that can be destroyed by jumping under it */
    BRICK     = 0x0004, //Marked for removal, DO NOT USE!
    /** the level should be finished when touching a goaltile.
     * if data is 0 then the endsequence should be triggered, if data is 1
     * then we can finish the level instantly.
     */
    GOAL      = 0x0008, //Marked for removal, DO NOT USE!
    /** slope tile */
    SLOPE     = 0x0010,
    /** Bonusbox, content is stored in \a data */
    FULLBOX   = 0x0020, //Marked for removal, DO NOT USE!
    /** Tile is a coin */
    COIN      = 0x0040, //Marked for removal, DO NOT USE!

    /* interesting flags (the following are passed to gameobjects) */
    FIRST_INTERESTING_FLAG = 0x0100,

    /** an ice brick that makes tux sliding more than usual */
    ICE       = 0x0100,
    /** a water tile in which tux starts to swim */
    WATER     = 0x0200,
    /** a tile that hurts Tux if he touches it */
    HURTS     = 0x0400,
    /** for lava: WATER, HURTS, FIRE */
    FIRE      = 0x0800,
    /** a walljumping trigger tile */
    WALLJUMP  = 0x1000
  };

  /** worldmap flags */
  enum {
    WORLDMAP_NORTH = 0x0001,
    WORLDMAP_SOUTH = 0x0002,
    WORLDMAP_EAST  = 0x0004,
    WORLDMAP_WEST  = 0x0008,
    WORLDMAP_DIR_MASK = 0x000f,

    WORLDMAP_STOP  = 0x0010,

    // convenience values ("C" stands for crossroads)
    WORLDMAP_CNSE  = WORLDMAP_NORTH | WORLDMAP_SOUTH | WORLDMAP_EAST,
    WORLDMAP_CNSW  = WORLDMAP_NORTH | WORLDMAP_SOUTH | WORLDMAP_WEST,
    WORLDMAP_CNEW  = WORLDMAP_NORTH | WORLDMAP_EAST  | WORLDMAP_WEST,
    WORLDMAP_CSEW  = WORLDMAP_SOUTH | WORLDMAP_EAST  | WORLDMAP_WEST,
    WORLDMAP_CNSEW = WORLDMAP_NORTH | WORLDMAP_SOUTH | WORLDMAP_EAST | WORLDMAP_WEST
  };

  enum
  {
    UNI_DIR_NORTH = 0,
    UNI_DIR_SOUTH = 1,
    UNI_DIR_WEST  = 2,
    UNI_DIR_EAST  = 3,
    UNI_DIR_MASK  = 3
  };

public:
  Tile();
  Tile(const std::vector<SurfacePtr>& images,
       const std::vector<SurfacePtr>& editor_images,
       uint32_t attributes, uint32_t data, float fps,
       bool deprecated = false,
       const std::string& obj_name = "", const std::string& obj_data = "");

  /** Draw a tile on the screen */
  void draw(Canvas& canvas, const Vector& pos, int z_pos, const Color& color = Color(1, 1, 1)) const;
  void draw_debug(Canvas& canvas, const Vector& pos, int z_pos, const Color& color = Color(1.0f, 0.f, 1.0f, 0.5f)) const;

  SurfacePtr get_current_surface() const;
  SurfacePtr get_current_editor_surface() const;

  inline uint32_t get_attributes() const { return m_attributes; }
  inline int get_data() const { return m_data; }

  /** Checks the SLOPE attribute. Returns "true" if set, "false" otherwise. */
  inline bool is_slope() const { return (m_attributes & SLOPE) != 0; }

  /** Determine the solidity of a tile. This version behaves correctly
      for unisolid tiles by taking position and movement of the object
      in question into account. Because creating the arguments for
      this function can be expensive, you should handle trivial cases
      using the "is_solid()" and "is_unisolid()" methods first. */
  bool is_solid (const Rectf& tile_bbox, const Rectf& position, const Vector& movement) const;

  /** This version only checks the SOLID flag to determine the
      solidity of a tile. This means it will always return "true" for
      unisolid tiles. To determine the *current* solidity of unisolid
      tiles, use the "is_solid" method that takes position and
      movement into account (see above). */
  inline bool is_solid() const { return (m_attributes & SOLID) != 0; }

  /** Determines whether the tile's attributes are important to
      calculate the collisions. The tile may be unisolid and therefore
      the collision with that tile don't matter.*/
  bool is_collisionful(const Rectf& tile_bbox, const Rectf& position, const Vector& movement) const;

  /** Checks the UNISOLID attribute. Returns "true" if set, "false" otherwise. */
  inline bool is_unisolid() const { return (m_attributes & UNISOLID) != 0; }

  inline bool is_deprecated() const { return m_deprecated; }

  inline const std::string& get_object_name() const { return m_object_name; }
  inline const std::string& get_object_data() const { return m_object_data; }

private:
  /** Returns zero if a unisolid tile is non-solid due to the movement
      direction, non-zero if the tile is solid due to direction. */
  bool check_movement_unisolid (const Vector& movement) const;

  /** Returns zero if a unisolid tile is non-solid due to the position
      of the tile and the object, non-zero if the tile is solid. */
  bool check_position_unisolid (const Rectf& obj_bbox,
                                const Rectf& tile_bbox) const;

private:
  std::vector<SurfacePtr> m_images;
  std::vector<SurfacePtr> m_editor_images;

  /** tile attributes */
  uint32_t m_attributes;

  /** General purpose data attached to a tile (content of a box, type of coin)*/
  int m_data;

  float m_fps;

  std::string m_object_name;
  std::string m_object_data;

  /** Prevent the addition of this tile to tilegroups, place restrictions in editor. */
  bool m_deprecated;

private:
  Tile(const Tile&) = delete;
  Tile& operator=(const Tile&) = delete;
};

#endif

/* EOF */
