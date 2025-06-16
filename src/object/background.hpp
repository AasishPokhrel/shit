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

#ifndef HEADER_SUPERTUX_OBJECT_BACKGROUND_HPP
#define HEADER_SUPERTUX_OBJECT_BACKGROUND_HPP

#include "editor/layer_object.hpp"

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/timer.hpp"
#include "video/blend.hpp"
#include "video/drawing_context.hpp"
#include "video/flip.hpp"

class ReaderMapping;

/**
 * @scripting
 * @summary A ""Background"" that was given a name can be manipulated by scripts.
 * @instances A ""Background"" can be accessed by its name from a script or via ""sector.name"" from the console.
 */
class Background final : public LayerObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  Background();
  Background(const ReaderMapping& reader);
  ~Background() override;

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  static std::string class_name() { return "background"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Background"; }
  static std::string display_name() { return _("Background"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(Background)); }

  virtual const std::string get_icon_path() const override {
    return "images/engine/editor/background.png";
  }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  virtual void on_flip(float height) override;

  void draw_image(DrawingContext& context, const Vector& pos);

  inline const std::string& get_image() const { return m_imagefile; }
  inline float get_speed() const { return m_parallax_speed.x; }
  int get_layer() const override { return m_layer; }

  inline Color get_color() const { return m_color; }
  void fade_color(Color color, float time);

  /**
   * @scripting
   * @description Sets the background's image.
   * @param string $image
   */
  void set_image(const std::string& image);
  /**
   * @scripting
   * @description Sets the top, middle and bottom background images.
   * @param string $top_image
   * @param string $middle_image
   * @param string $bottom_image
   */
  void set_images(const std::string& top_image, const std::string& middle_image,
                  const std::string& bottom_image);
  /**
   * @scripting
   * @description Sets the background speed.
   * @param float $speed
   */
  void set_speed(float speed);

  /**
   * @scripting
   * @description Returns the red color value.
   */
  inline float get_color_red() const { return m_color.red; }
  /**
   * @scripting
   * @description Returns the green color value.
   */
  inline float get_color_green() const { return m_color.green; }
  /**
   * @scripting
   * @description Returns the blue color value.
   */
  inline float get_color_blue() const { return m_color.blue; }
  /**
   * @scripting
   * @description Returns the alpha color value.
   */
  inline float get_color_alpha() const { return m_color.alpha; }
  /**
   * @scripting
   * @description Sets the background color.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   */
  inline void set_color(float red, float green, float blue, float alpha) { m_color = Color(red, green, blue, alpha); }
  /**
   * @scripting
   * @description Fades to specified background color in ""time"" seconds.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   * @param float $time
   */
  inline void fade_color(float red, float green, float blue, float alpha, float time) { fade_color(Color(red, green, blue, alpha), time); }
  /**
   * Sets the sprite action for the top image.
   * @param string $action
   */
  void set_top_image_action(const std::string& action);
  /**
   * Sets the sprite action for the main (middle) image.
   * @param string $action
   */
  void set_image_action(const std::string& action);
  /**
   * Sets the sprite action for the bottom image.
   * @param string $action
   */
  void set_bottom_image_action(const std::string& action);
  /**
   * Sets the sprite action for all images (top, middle and bottom).
   * @param string $action
   */
  void set_all_image_actions(const std::string& action);

private:
  enum Alignment {
    NO_ALIGNMENT,
    LEFT_ALIGNMENT,
    RIGHT_ALIGNMENT,
    TOP_ALIGNMENT,
    BOTTOM_ALIGNMENT
  };

private:
  /** Backgrounds with NO_ALIGNMENT are repeated over the whole
      screen, backgrounds with left, right, top, bottom alignment are
      only repeated in one direction and attached to the level edge. */
  Alignment m_alignment;

  /** If fill is set, the background will not repeat and is instead
      stretched over the whole screen, alignment and top/bottom images
      are ignored in that case. */
  bool m_fill;

  int m_layer;
  std::string m_imagefile_top;
  std::string m_imagefile;
  std::string m_imagefile_bottom;
  Vector m_pos; /**< coordinates of upper-left corner of image */
  Vector m_parallax_speed;
  Vector m_scroll_speed;
  Vector m_scroll_offset;
  SpritePtr m_image_top; /**< image to draw above pos */
  SpritePtr m_image; /**< image to draw, anchored at pos */
  SpritePtr m_image_bottom; /**< image to draw below pos+screenheight */

  Blend m_blend;
  Color m_color;
  DrawingTarget m_target;

  Timer m_timer_color;
  Color m_src_color, m_dst_color;

  Flip m_flip;

private:
  Background(const Background&) = delete;
  Background& operator=(const Background&) = delete;
};

#endif

/* EOF */
