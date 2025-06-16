//  SuperTux - MovingSprite Base Class
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_MOVING_SPRITE_HPP
#define HEADER_SUPERTUX_OBJECT_MOVING_SPRITE_HPP

#include "math/anchor_point.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/moving_object.hpp"
#include "video/drawing_context.hpp"
#include "video/flip.hpp"

class ReaderMapping;

/**
 * @scripting
 * @summary Abstract base class for ""MovingObject""s, that are represented by a sprite.
 */
class MovingSprite : public MovingObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  MovingSprite(const Vector& pos,
               const std::string& sprite_name,
               int layer = LAYER_OBJECTS,
               CollisionGroup collision_group = COLGROUP_MOVING);
  MovingSprite(const ReaderMapping& reader,
               const Vector& pos,
               int layer = LAYER_OBJECTS,
               CollisionGroup collision_group = COLGROUP_MOVING);
  MovingSprite(const ReaderMapping& reader,
               const std::string& sprite_name,
               int layer = LAYER_OBJECTS,
               CollisionGroup collision_group = COLGROUP_MOVING);
  MovingSprite(const ReaderMapping& reader,
               int layer = LAYER_OBJECTS,
               CollisionGroup collision_group = COLGROUP_MOVING);

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  static std::string class_name() { return "moving-sprite"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "MovingSprite"; }
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(MovingSprite)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;
  virtual void on_type_change(int old_type) override;

  int get_layer() const override { return m_layer; }
  void set_layer(int layer) { m_layer = layer; }

  inline bool has_found_sprite() const { return m_sprite_found; }
  inline const std::string& get_sprite_name() const { return m_sprite_name; }
  virtual std::string get_default_sprite_name() const { return m_default_sprite_name; }

  bool matches_sprite(const std::string& sprite_file) const;
  bool change_sprite(const std::string& new_sprite_name);
  void spawn_explosion_sprites(int count, const std::string& sprite_path);

  /** Get various sprite properties. **/
  inline Sprite* get_sprite() const { return m_sprite.get(); }

  /** "void" wrapper for "change_sprite()" to be used for the "sprite" scripting property. **/
  inline void set_sprite(const std::string& file) { change_sprite(file); }

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description Sets the sprite of the object. Returns ""true"" on success.
   * @param string $file
   */
  bool set_sprite(const std::string& file);
  /**
   * @scripting
   * @description Returns the file of the object's sprite.
   */
  std::string get_sprite() const;
#endif
  /**
   * @scripting
   * @description Returns the name of the current action of the sprite.
   */
  std::string get_action() const;
  /**
   * @scripting
   * @description Sets the current action of the sprite, as well as the number of times it should loop, and resizes the bounding box.
                  NOTE: Use with care as you can easily get stuck when resizing the bounding box.
   * @param string $name
   * @param int $loops Optional, -1 by default (negative value means infinite). Set to -100 to continue looping from the previous action.
   */
  void set_action(const std::string& name, int loops = -1);
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @deprecated Use ""set_action()"" instead!
   * @description Sets the current action of the sprite, as well as the number of times it should loop, and resizes the bounding box.
                  NOTE: Use with care as you can easily get stuck when resizing the bounding box.
   * @param string $name
   * @param int $loops Negative value means infinite. Set to -100 to continue looping from the previous action.
   */
  void set_action_loops(const std::string& name, int loops);
#endif

  /** Sets the action from an action name and a particular direction
      in the form of "name-direction", eg. "walk-left".
   */
  void set_action(const std::string& name, const Direction& dir, int loops = -1);

  /** Sets the action from an action name and a particular direction
      in the form of "direction-name", eg. "left-up".
   */
  void set_action(const Direction& dir, const std::string& name, int loops = -1);

  /** Sets the action from a string from a particular direction
      in the form of "direction", e.g. "left".
   */
  void set_action(const Direction& dir, int loops = -1);

  /** Set new action for sprite and re-center bounding box.  use with
      care as you can easily get stuck when resizing the bounding
      box. */
  void set_action_centered(const std::string& action, int loops = -1);

  /** Set new action for sprite and align bounding boxes at
      anchorPoint.  use with care as you can easily get stuck when
      resizing the bounding box. */
  void set_action(const std::string& action, int loops, AnchorPoint anchorPoint);

protected:
  /** Update hitbox, based on sprite. */
  virtual void update_hitbox();

protected:
  std::string m_sprite_name;

  /** The default sprite for this MovingObject.
      NOTE: Unless in a constructor, use get_default_sprite_name() instead,
            so support for sprite switching for object types is retained. */
  std::string m_default_sprite_name;
  SpritePtr m_sprite;
  int m_layer; /**< Sprite's z-position. Refer to video/drawing_context.hpp for sensible values. */

  Flip m_flip;

private:
  /** A custom sprite has been successfully found and set on initialization. */
  bool m_sprite_found;

  /** A custom layer has been specified. */
  const bool m_custom_layer;

private:
  MovingSprite(const MovingSprite&) = delete;
  MovingSprite& operator=(const MovingSprite&) = delete;
};

#endif

/* EOF */
