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

#ifndef HEADER_SUPERTUX_OBJECT_TILEMAP_HPP
#define HEADER_SUPERTUX_OBJECT_TILEMAP_HPP

#include "editor/layer_object.hpp"

#include <algorithm>
#include <unordered_set>

#include "math/rect.hpp"
#include "math/rectf.hpp"
#include "math/size.hpp"
#include "object/path_object.hpp"
#include "object/path_walker.hpp"
#include "supertux/autotile.hpp"
#include "video/color.hpp"
#include "video/flip.hpp"
#include "video/drawing_target.hpp"

class AutotileSet;
class CollisionObject;
class CollisionGroundMovementManager;
class DrawingContext;
class Tile;
class TileSet;

/**
 * This class is responsible for managing an array of tiles.

 * @scripting
 * @summary A ""TileMap"" that was given a name can be controlled by scripts.
            The tilemap can be moved by specifying a path for it.
 * @instances A ""TileMap"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class TileMap final : public LayerObject,
                      public PathObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  TileMap(const TileSet *tileset);
  TileMap(const TileSet *tileset, const ReaderMapping& reader);
  ~TileMap() override;

  virtual void finish_construction() override;

  static std::string class_name() { return "tilemap"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "TileMap"; }
  virtual const std::string get_icon_path() const override;
  static std::string display_name() { return _("Tilemap"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(PathObject)).add(typeid(TileMap)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  void save_state() override;
  void check_state() override;

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  void on_path_resolved() override;

  virtual void editor_update() override;

  virtual void on_flip(float height) override;

  void parse_tiles(const ReaderMapping& reader);
  void write_tiles(Writer& writer) const;

  void set(int width, int height, const std::vector<unsigned int>& vec,
           int z_pos, bool solid);

  /** resizes the tilemap to a new width and height (tries to not
      destroy the existing map) */
  void resize(int newwidth, int newheight, int fill_id = 0,
              int xoffset = 0, int yoffset = 0);
  void resize(const Size& newsize, const Size& resize_offset);

  inline int get_width() const { return m_width; }
  inline int get_height() const { return m_height; }
  inline Size get_size() const { return Size(m_width, m_height); }

  inline void set_offset(const Vector &offset_) { m_offset = offset_; }
  inline Vector get_offset() const { return m_offset; }

  void set_ground_movement_manager(const std::shared_ptr<CollisionGroundMovementManager>& movement_manager)
  {
    m_ground_movement_manager = movement_manager;
  }

  void move_by(const Vector& pos);

  /** Get the movement of this tilemap. The collision detection code
      may need a non-negative y-movement. Passing `false' as the
      `actual' argument will provide that. Used exclusively in
      src/supertux/sector.cpp. */
  Vector get_movement(bool actual) const
  {
    if (actual) {
      return m_movement;
    }

    return Vector(m_movement.x, std::max(0.0f, m_movement.y));
  }

  /** Returns the position of the upper-left corner of tile (x, y) in
      the sector. */
  Vector get_tile_position(int x, int y) const
  { return m_offset + Vector(static_cast<float>(x), static_cast<float>(y)) * 32.0f; }

  Rectf get_bbox() const {
    return Rectf(get_tile_position(0, 0),
                 get_tile_position(m_width, m_height));
  }

  Rectf get_tile_bbox(int x, int y) const {
    return Rectf(get_tile_position(x, y),
                 get_tile_position(x + 1, y + 1));
  }

  /** Returns the half-open rectangle of (x, y) tile indices that
      overlap the given rectangle in the sector. */
  Rect get_tiles_overlapping(const Rectf &rect) const;

  /** Called by the collision mechanism to indicate that this tilemap has been hit on
      the top, i.e. has hit a moving object on the bottom of its collision rectangle. */
  void hits_object_bottom(CollisionObject& object);
  void notify_object_removal(CollisionObject* other);

  int get_layer() const override { return m_z_pos; }
  inline void set_layer(int layer) { m_z_pos = layer; }

  inline bool is_solid() const { return m_real_solid && m_effective_solid; }

  /**
   * @scripting
   * @description Switches the tilemap's real solidity to ""solid"".${SRG_TABLENEWPARAGRAPH}
                  Note: The effective solidity is also influenced by the alpha of the tilemap.
   * @param bool $solid
   */
  void set_solid(bool solid = true);
  /**
   * @scripting
   * @description Returns the effective solidity of the tilemap.
   */
  inline bool get_solid() const { return m_effective_solid; }

  bool is_outside_bounds(const Vector& pos) const;
  const Tile& get_tile(int x, int y) const;
  const Tile& get_tile_at(const Vector& pos) const;
  /**
   * @scripting
   * @description Returns the ID of the tile at the given coordinates or 0 if out of bounds.
                  The origin is at the top left.
   * @param int $x
   * @param int $y
   */
  uint32_t get_tile_id(int x, int y) const;
  uint32_t get_tile_id(const Vector& pos) const;
  /**
   * @scripting
   * @description Returns the ID of the tile at the given position (in world coordinates).
   * @param float $x
   * @param float $y
   */
  uint32_t get_tile_id_at(float x, float y) const;
  uint32_t get_tile_id_at(const Vector& pos) const;

  /**
   * @scripting
   * @description Changes the tile at the given coordinates to ""newtile"".
                  The origin is at the top left.
   * @param int $x
   * @param int $y
   * @param int $newtile
   */
  void change(int x, int y, uint32_t newtile);
  void change(int idx, uint32_t newtile);
  /**
   * @scripting
   * @description Changes the tile at the given position (in-world coordinates) to ""newtile"".
   * @param float $x
   * @param float $y
   * @param int $newtile
   */
  void change_at(float x, float y, uint32_t newtile);
  void change_at(const Vector& pos, uint32_t newtile);
  /**
   * @scripting
   * @description Changes all tiles with the given ID.
   * @param int $oldtile
   * @param int $newtile
   */
  void change_all(uint32_t oldtile, uint32_t newtile);

  /** Puts the correct autotile blocks at the given position */
  void autotile(const Vector& pos, uint32_t tile, AutotileSet* autotileset);

  /** Erases in autotile mode */
  void autotile_erase(const Vector& pos, AutotileSet* autotileset);

  /** Returns the Autotilesets associated with the given tile */
  std::vector<AutotileSet*> get_autotilesets(uint32_t tile) const;

  inline void set_flip(Flip flip) { m_flip = flip; }
  inline Flip get_flip() const { return m_flip; }

  /**
   * @scripting
   * @description Starts fading the tilemap to the opacity given by ""alpha"".
                  Destination opacity will be reached after ""time"" seconds. Also influences solidity.
   * @param float $alpha
   * @param float $time
   */
  void fade(float alpha, float time);

  /** Start fading the tilemap to tint given by RGBA.
      Destination opacity will be reached after @c seconds seconds. Doesn't influence solidity. */
  void tint_fade(const Color& new_tint, float time = 0.f);
  /**
   * @scripting
   * @description Starts fading the tilemap to tint given by RGBA.
                  Destination opacity will be reached after ""time"" seconds. Doesn't influence solidity.
   * @param float $time
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   */
  void tint_fade(float time, float red, float green, float blue, float alpha);

  inline Color get_current_tint() const { return m_current_tint; }

  /**
   * @scripting
   * @description Instantly switches the tilemap's opacity to ""alpha"". Also influences solidity.
   * @param float $alpha
   */
  void set_alpha(float alpha);
  /**
   * @scripting
   * @description Returns the tilemap's opacity.${SRG_TABLENEWPARAGRAPH}
                  Note: While the tilemap is fading in or out, this will return the current alpha value, not the target alpha.
   */
  float get_alpha() const;

  inline float get_target_alpha() const { return m_alpha; }

  inline void set_tileset(const TileSet* tileset) { m_tileset = tileset; }

  inline const std::vector<uint32_t>& get_tiles() const { return m_tiles; }

private:
  void update_effective_solid(bool update_manager = true);
  void float_channel(float target, float &current, float remaining_time, float dt_sec);

  /** Puts the correct single autotile block at the given position */
  void autotile_single(int x, int y, AutotileSet* autotileset);

  enum class AutotileCornerOperation {
    ADD_TOP_LEFT,
    ADD_TOP_RIGHT,
    ADD_BOTTOM_LEFT,
    ADD_BOTTOM_RIGHT,
    REMOVE_TOP_LEFT,
    REMOVE_TOP_RIGHT,
    REMOVE_BOTTOM_LEFT,
    REMOVE_BOTTOM_RIGHT,
  };

  /** Puts the correct autotile blocks at the tiles around the single given corner */
  void autotile_single_corner(int x, int y, AutotileSet* autotileset, AutotileCornerOperation op);

  void apply_offset_x(int fill_id, int xoffset);
  void apply_offset_y(int fill_id, int yoffset);

public:
  bool m_editor_active;

private:
  const TileSet* m_tileset;

  typedef std::vector<uint32_t> Tiles;
  Tiles m_tiles;

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description Equivalent to ""get_solid()"" and ""set_solid()"".
   */
  bool m_solid;
#endif

  /* read solid: In *general*, is this a solid layer? effective solid:
     is the layer *currently* solid? A generally solid layer may be
     not solid when its alpha is low. See `is_solid' above. */
  bool m_real_solid;
  bool m_effective_solid;

  float m_speed_x;
  float m_speed_y;
  int m_width;
  int m_height;
  int m_z_pos;
  Vector m_offset;
  Vector m_movement; /**< The movement that happened last frame */

  /** Objects that were touching the top of a solid tile at the last frame */
  std::unordered_set<CollisionObject*> m_objects_hit_bottom;

  std::shared_ptr<CollisionGroundMovementManager> m_ground_movement_manager;

  Flip m_flip;
  /**
   * @scripting
   * @description Determines the tilemap's current opacity.
   */
  float m_alpha; /**< requested tilemap opacity */
  float m_current_alpha; /**< current tilemap opacity */
  float m_remaining_fade_time; /**< seconds until requested tilemap opacity is reached */

  /** The tint can have its own alpha channel, but this alpha channel doesn't affect
      the solidity of the tilemap. This alpha channel makes the tilemap only less or
      more translucent.*/
  Color m_tint; /**< requested tilemap tint */
  Color m_current_tint; /**< current tilemap tint */
  float m_remaining_tint_fade_time; /**< seconds until requested tilemap tint is reached */

  /** Set to LIGHTMAP to draw to lightmap */
  DrawingTarget m_draw_target;

  int m_new_size_x;
  int m_new_size_y;
  int m_new_offset_x;
  int m_new_offset_y;
  bool m_add_path;

  int m_starting_node;

private:
  TileMap(const TileMap&) = delete;
  TileMap& operator=(const TileMap&) = delete;
};

#endif

/* EOF */
