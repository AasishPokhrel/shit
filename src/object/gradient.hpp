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

#ifndef HEADER_SUPERTUX_OBJECT_GRADIENT_HPP
#define HEADER_SUPERTUX_OBJECT_GRADIENT_HPP

#include "editor/layer_object.hpp"

#include "video/drawing_context.hpp"

class ReaderMapping;

/**
 * @scripting
 * @summary A ""Gradient"" that was given a name can be controlled by scripts.
 * @instances A ""Gradient"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class Gradient final : public LayerObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  Gradient();
  Gradient(const ReaderMapping& reader);
  ~Gradient() override;

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual bool is_saveable() const override;

  static std::string class_name() { return "gradient"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Gradient"; }
  static std::string display_name() { return _("Gradient"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(Gradient)); }

  virtual const std::string get_icon_path() const override {
    return "images/engine/editor/gradient.png";
  }

  virtual ObjectSettings get_settings() override;

  virtual void on_flip(float height) override;

  void set_gradient(const Color& top, const Color& bottom);
  void fade_gradient(const Color& top, const Color& bottom, float time);
  inline Color get_gradient_top() const { return m_gradient_top; }
  inline Color get_gradient_bottom() const { return m_gradient_bottom; }

  inline GradientDirection get_direction() const { return m_gradient_direction; }
  std::string get_direction_string() const;
  inline void set_direction(const GradientDirection& direction) { m_gradient_direction = direction; }

  inline void set_layer(int layer) { m_layer = layer; }
  int get_layer() const override { return m_layer; }

  /**
   * @scripting
   * @description Sets the direction of the gradient.
   * @param string $direction Can be "horizontal", "vertical", "horizontal_sector" or "vertical_sector".
   */
  void set_direction(const std::string& direction);
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description Returns the direction of the gradient.
                  Possible values are "horizontal", "vertical", "horizontal_sector" or "vertical_sector".
   */
  std::string get_direction() const;
#endif
  /**
   * @scripting
   * @description Set top gradient color.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   */
  void set_color1(float red, float green, float blue, float alpha = 1.f);
  /**
   * @scripting
   * @description Set bottom gradient color.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $alpha
   */
  void set_color2(float red, float green, float blue, float alpha = 1.f);
  /**
   * @scripting
   * @description Set both gradient colors.
   * @param float $red1
   * @param float $green1
   * @param float $blue1
   * @param float $red2
   * @param float $green2
   * @param float $blue2
   * @param float $alpha1
   * @param float $alpha2
   */
  void set_colors(float red1, float green1, float blue1, float red2, float green2, float blue2, float alpha1 = 1.f, float alpha2 = 1.f);
  /**
   * @scripting
   * @description Fade the top gradient color to a specified new color in ""time"" seconds.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $time
   * @param float $alpha
   */
  void fade_color1(float red, float green, float blue, float time, float alpha = 1.f);
  /**
   * @scripting
   * @description Fade the bottom gradient color to a specified new color in ""time"" seconds.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $time
   * @param float $alpha
   */
  void fade_color2(float red, float green, float blue, float time, float alpha = 1.f);
  /**
   * @scripting
   * @description Fade both gradient colors to specified new colors in ""time"" seconds.
   * @param float $red1
   * @param float $green1
   * @param float $blue1
   * @param float $red2
   * @param float $green2
   * @param float $blue2
   * @param float $time
   * @param float $alpha1
   * @param float $alpha2
   */
  void fade_colors(float red1, float green1, float blue1, float red2, float green2, float blue2, float time, float alpha1 = 1.f, float alpha2 = 1.f);
  /**
   * @scripting
   * @description Swap top and bottom gradient colors.
   */
  void swap_colors();

private:
  int m_layer;
  Color m_gradient_top;
  Color m_gradient_bottom;
  GradientDirection m_gradient_direction;
  Blend m_blend;
  DrawingTarget m_target;

  Color m_start_gradient_top;
  Color m_start_gradient_bottom;
  Color m_fade_gradient_top;
  Color m_fade_gradient_bottom;
  float m_fade_total_time;
  float m_fade_time;

public:
  static const Color DEFAULT_GRADIENT_TOP;
  static const Color DEFAULT_GRADIENT_BOTTOM;

private:
  Gradient(const Gradient&) = delete;
  Gradient& operator=(const Gradient&) = delete;
};

#endif

/* EOF */
