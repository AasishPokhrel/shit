//  SuperTux -  A Jump'n Run
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

#ifndef HEADER_SUPERTUX_SUPERTUX_SECTOR_HPP
#define HEADER_SUPERTUX_SUPERTUX_SECTOR_HPP

#include "supertux/sector_base.hpp"

#include <vector>
#include <stdint.h>

#include "collision/collision_system.hpp"
#include "math/anchor_point.hpp"
#include "math/easing.hpp"
#include "math/fwd.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/tile.hpp"
#include "video/color.hpp"

namespace collision {
class Constraints;
}

class Camera;
class CollisionGroundMovementManager;
class DisplayEffect;
class DrawingContext;
class Level;
class MovingObject;
class Player;
class ReaderMapping;
class Rectf;
class Size;
class SpawnPointMarker;
class TextObject;
class TileMap;
class Writer;

/**
 * Represents one of (potentially) multiple, separate parts of a Level.
   Sectors contain GameObjects, e.g. Badguys and Players.
 */
/**
 * @scripting
 * @summary This class provides additional controlling functions for a sector, other than the ones listed at ${SRG_REF_GameObjectManager}.
 * @instances An instance under ""sector.settings"" is available from scripts and the console.
 */
class Sector final : public Base::Sector
{
  friend class CollisionSystem;
  friend class EditorSectorMenu;

public:
  static void register_class(ssq::VM& vm);

private:
  static Sector* s_current;

public:
  /** get currently activated sector. */
  static Sector& get() { assert(s_current != nullptr); return *s_current; }
  static Sector* current() { return s_current; }

public:
  Sector(Level& parent);
  ~Sector() override;

  void finish_construction(bool editable) override;

  std::string get_exposed_class_name() const override { return "Sector"; }

  inline Level& get_level() const { return m_level; }
  TileSet* get_tileset() const override;
  bool in_worldmap() const override;

  /** activates this sector (change music, initialize player class, ...) */
  void activate(const std::string& spawnpoint);
  void activate(const Vector& player_pos);
  void deactivate();

  void draw(DrawingContext& context) override;
  void update(float dt_sec) override;

  void save(Writer &writer);

  /** stops all looping sounds in whole sector. */
  void stop_looping_sounds();

  /** Freeze camera position for this frame, preventing camera interpolation jumps and loops */
  void pause_camera_interpolation();

  /** continues the looping sounds in whole sector. */
  void play_looping_sounds();

  /** tests if a given rectangle is inside the sector
      (a rectangle that is on top of the sector is considered inside) */
  bool inside(const Rectf& rectangle) const;

  /** Checks if the specified rectangle is free of (solid) tiles.
      Note that this does not include static objects, e.g. bonus blocks. */
  bool is_free_of_tiles(const Rectf& rect, const bool ignoreUnisolid = false, uint32_t tiletype = Tile::SOLID) const;
  /**
   * @scripting
   * @description Checks if the specified sector-relative rectangle is free of solid tiles.
   * @param float $left
   * @param float $top
   * @param float $right
   * @param float $bottom
   * @param bool $ignore_unisolid If ""true"", unisolid tiles will be ignored.
   */
  bool is_free_of_solid_tiles(float left, float top, float right, float bottom,
                              bool ignore_unisolid) const;

  /** Checks if the specified rectangle is free of both
      1.) solid tiles and
      2.) MovingObjects in COLGROUP_STATIC.
      Note that this does not include badguys or players. */
  bool is_free_of_statics(const Rectf& rect, const MovingObject* ignore_object = nullptr,
                          const bool ignoreUnisolid = false, uint32_t tiletype = Tile::SOLID) const;
  /**
   * @scripting
   * @description Checks if the specified sector-relative rectangle is free of both:
                    1) Solid tiles.
                    2) ""MovingObject""s in ""COLGROUP_STATIC"".
                  Note: This does not include badguys or players.
   * @param float $left
   * @param float $top
   * @param float $right
   * @param float $bottom
   * @param bool $ignore_unisolid If ""true"", unisolid tiles will be ignored.
   */
  bool is_free_of_statics(float left, float top, float right, float bottom,
                          bool ignore_unisolid) const;

  /** Checks if the specified rectangle is free of both
      1.) solid tiles and
      2.) MovingObjects in COLGROUP_STATIC, COLGROUP_MOVINGSTATIC or COLGROUP_MOVING.
      This includes badguys and players. */
  bool is_free_of_movingstatics(const Rectf& rect, const MovingObject* ignore_object = nullptr,
                                bool ignore_unisolid = false) const;
  /**
   * @scripting
   * @description Checks if the specified sector-relative rectangle is free of both:
                    1) Solid tiles.
                    2) ""MovingObject""s in ""COLGROUP_STATIC"", ""COLGROUP_MOVINGSTATIC"" or ""COLGROUP_MOVING"".
                  This includes badguys and players.
   * @param float $left
   * @param float $top
   * @param float $right
   * @param float $bottom
   */
  bool is_free_of_movingstatics(float left, float top, float right, float bottom) const;

  /** Checks if the specified rectangle is free of MovingObjects in COLGROUP_MOVINGSTATIC.
      Note that this does not include moving badguys, or players */
  bool is_free_of_specifically_movingstatics(const Rectf& rect, const MovingObject* ignore_object = nullptr) const;
  /**
   * @scripting
   * @description Checks if the specified sector-relative rectangle is free of ""MovingObject""s in ""COLGROUP_MOVINGSTATIC"".
                  Note: This does not include moving badguys or players.
   * @param float $left
   * @param float $top
   * @param float $right
   * @param float $bottom
   */
  bool is_free_of_specifically_movingstatics(float left, float top, float right, float bottom) const;

  CollisionSystem::RaycastResult get_first_line_intersection(const Vector& line_start,
                                                             const Vector& line_end,
                                                             CollisionSystem::RaycastIgnore ignore,
                                                             const CollisionObject* ignore_object) const;

  CollisionSystem::RaycastResult get_first_line_intersection(const Vector& line_start,
                                                             const Vector& line_end,
                                                             bool ignore_objects,
                                                             const CollisionObject* ignore_object) const;

  bool free_line_of_sight(const Vector& line_start, const Vector& line_end, bool ignore_objects = false, const MovingObject* ignore_object = nullptr) const;
  bool can_see_player(const Vector& eye) const;

  Player* get_nearest_player(const Vector& pos) const;
  Player* get_nearest_player(const Rectf& pos) const
  {
    return get_nearest_player(get_anchor_pos(pos, ANCHOR_MIDDLE));
  }

  std::vector<MovingObject*> get_nearby_objects (const Vector& center, float max_distance) const;

  Rectf get_active_region() const;

  inline int get_foremost_opaque_layer() const { return m_foremost_opaque_layer; }
  inline int get_foremost_layer() const { return m_foremost_layer; }

  /** returns the editor size (in tiles) of a sector */
  Size get_editor_size() const;

  /** resize all tilemaps with given size */
  void resize(const Size& old_size, const Size& new_size, const Size& resize_offset);

  /** globally changes solid tilemaps' tile ids */
  void change_solid_tiles(uint32_t old_tile_id, uint32_t new_tile_id);

  /**
   * @scripting
   * Sets the sector's gravity.
   * @param float $gravity
   */
  inline void set_gravity(float gravity) { m_gravity = gravity; }
  /**
   * @scripting
   * Returns the sector's gravity.
   * @param float $gravity
   */
  inline float get_gravity() const { return m_gravity; }

  Camera& get_camera() const;
  DisplayEffect& get_effect() const;
  inline TextObject& get_text_object() const { return m_text_object; }

  std::vector<Player*> get_players() const;

  Vector get_spawn_point_position(const std::string& spawnpoint);

private:
  uint32_t collision_tile_attributes(const Rectf& dest, const Vector& mov) const;

  virtual bool before_object_add(GameObject& object) override;
  virtual void before_object_remove(GameObject& object) override;

  int calculate_foremost_layer(bool including_transparent = true) const;

  /** Convert tiles into their corresponding GameObjects (e.g.
      bonusblocks, add light to lava tiles) */
  void convert_tiles2gameobject();

  SpawnPointMarker* get_spawn_point(const std::string& spawnpoint);

private:
  Level& m_level; // Parent level

  int m_foremost_layer;
  int m_foremost_opaque_layer;

  /**
   * @scripting
   * @description The sector's gravity.
   */
  float m_gravity;

  std::unique_ptr<CollisionSystem> m_collision_system;

  TextObject& m_text_object;

  Vector m_last_translation; // For camera interpolation at high frame rates
  float m_last_scale;
  float m_last_dt;

private:
  Sector(const Sector&) = delete;
  Sector& operator=(const Sector&) = delete;
};

#endif

/* EOF */
