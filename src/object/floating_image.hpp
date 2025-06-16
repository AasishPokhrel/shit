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

#ifndef HEADER_SUPERTUX_OBJECT_FLOATING_IMAGE_HPP
#define HEADER_SUPERTUX_OBJECT_FLOATING_IMAGE_HPP

#include "math/anchor_point.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"

/**
 * @scripting
 * @summary This class provides the ability to create, edit, and remove images floating in midair on the screen.
            It is implemented as a wrapper around a sprite, so any sprite actions are applicable.
 * @instances Floating Images are created in a script or from the console. Constructor:${SRG_NEWPARAGRAPH}
              """"""<floatimage> <- FloatingImage(string filename)""""""${SRG_NEWPARAGRAPH}
              This creates a ""FloatingImage"" from ""filename"" (which is relative to the data directory root).
 */
class FloatingImage final : public GameObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  FloatingImage(const std::string& sprite);
  ~FloatingImage() override;

  virtual bool is_saveable() const override { return false; }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(FloatingImage)); }
  virtual std::string get_exposed_class_name() const override { return "FloatingImage"; }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  /**
   * @scripting
   * @description Sets the layer of the floating image.
   * @param int $layer
   */
  inline void set_layer(int layer) { m_layer = layer; }
  /**
   * @scripting
   * @description Returns the layer the floating image is on.
   */
  inline int get_layer() const { return m_layer; }
  /**
   * @scripting
   * @description Sets the location of the image in relation to the current anchor point.
   * @param float $x
   * @param float $y
   */
  inline void set_pos(float x, float y) { m_pos = Vector(x, y); }
  /**
   * @scripting
   * @description Returns the image's X coordinate relative to the current anchor point.
   */
  inline float get_x() const { return m_pos.x; }
  /**
   * @scripting
   * @description Returns the image's Y coordinate relative to the current anchor point.
   */
  inline float get_y() const { return m_pos.y; }
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @deprecated Use ""get_x()"" instead!
   * @description Returns the image's X coordinate relative to the current anchor point.
   */
  float get_pos_x() const;
  /**
   * @scripting
   * @deprecated Use ""get_y()"" instead!
   * @description Returns the image's Y coordinate relative to the current anchor point.
   */
  float get_pos_y() const;
#endif
  /**
   * @scripting
   * @description Sets the image's anchor point.
   * @param int $anchor Anchor point as represented by the ""ANCHOR_*"" constants (see ${SRG_REF_AnchorPoints}).
   */
  inline void set_anchor_point(int anchor) { m_anchor = static_cast<AnchorPoint>(anchor); }
  /**
   * @scripting
   * @description Returns the current anchor point.
   */
  inline int get_anchor_point() const { return static_cast<int>(m_anchor); }
  /**
   * @scripting
   * @description Sets the visibility of the floating image.
   * @param bool $visible
   */
  inline void set_visible(bool visible) { m_visible = visible; }
  /**
   * @scripting
   * @description Returns the visibility state of the floating image.
   */
  inline bool get_visible() const { return m_visible; }
  /**
   * @scripting
   * @description Sets the action of the image.
   *              This is only useful when the image is a sprite.
   * @param string $action Name of the action, as defined in the sprite.
   */
  void set_action(const std::string& action);
  /**
   * @scripting
   * @description Returns the name of the action of the image, as defined in the sprite.
   * This is only useful when the image is a sprite.
   */
  std::string get_action() const;
  /**
   * @scripting
   * @description Fades in the image for the next ""time"" seconds.
   * @param float $time
   */
  void fade_in(float time);
  /**
   * @scripting
   * @description Fades out the image for the next ""time"" seconds.
   * @param float $time
   */
  void fade_out(float time);

private:
  SpritePtr m_sprite;
  /**
   * @scripting
   * The layer the floating image is on.
   */
  int m_layer;
  /**
   * @scripting
   * Determines whether the floating image is visible.
   */
  bool m_visible;
  AnchorPoint m_anchor;
  Vector m_pos;
  float m_fading;
  float m_fadetime;

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description The current anchor point.
   */
  int m_anchor_point;
#endif
};

#endif

/* EOF */
