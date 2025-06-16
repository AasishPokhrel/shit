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

#ifndef HEADER_SUPERTUX_OBJECT_SPOTLIGHT_HPP
#define HEADER_SUPERTUX_OBJECT_SPOTLIGHT_HPP

#include "sprite/sprite_ptr.hpp"
#include "supertux/moving_object.hpp"
#include "video/color.hpp"

class ReaderMapping;

/**
 * @scripting
 * @summary A ""Spotlight"" that was given a name can be controlled by scripts.
 * @instances A ""Spotlight"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class Spotlight final : public MovingObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  enum class Direction {
    CLOCKWISE,
    COUNTERCLOCKWISE,
    STOPPED
  };

  static Direction Direction_from_string(const std::string& s);
  static std::string Direction_to_string(Direction dir);

public:
  Spotlight(const ReaderMapping& reader);
  ~Spotlight() override;

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit_) override;

  static std::string class_name() { return "spotlight"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Spotlight"; }
  static std::string display_name() { return _("Spotlight"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingObject::get_class_types().add(typeid(Spotlight)); }

  virtual ObjectSettings get_settings() override;

  virtual int get_layer() const override { return m_layer; }

  /**
   * @scripting
   * @description Enables/disables the spotlight.
   * @param bool $enabled
   */
  inline void set_enabled(bool enabled) { m_enabled = enabled; }
  /**
   * @scripting
   * @description Returns ""true"" if the spotlight is enabled.
   */
  inline bool is_enabled() { return m_enabled; }

  /**
   * @scripting
   * @description Sets the direction of the spotlight.
   * @param string $direction
   */
  void set_direction(const std::string& direction);

  /**
   * @scripting
   * @description Sets the angle of the spotlight.
   * @param float $angle
   */
  inline void set_angle(float angle) { m_angle = angle; }
  /**
   * @scripting
   * @description Fades the angle of the spotlight in ""time"" seconds.
   * @param float $angle
   * @param float $time
   */
  inline void fade_angle(float angle, float time) { ease_angle(time, angle); }
  /**
   * @scripting
   * @description Fades the angle of the spotlight in ""time"" seconds, with easing.
   * @param float $angle
   * @param float $time
   * @param string $easing
   */
  void ease_angle(float angle, float time, const std::string& easing);

  /**
   * @scripting
   * @description Sets the speed of the spotlight.
   * @param float $speed
   */
  inline void set_speed(float speed) { m_speed = speed; }
  /**
   * @scripting
   * @description Fades the speed of the spotlight in ""time"" seconds.
   * @param float $speed
   * @param float $time
   */
  inline void fade_speed(float speed, float time) { ease_speed(time, speed); }
  /**
   * @scripting
   * @description Fades the speed of the spotlight in ""time"" seconds, with easing.
   * @param float $speed
   * @param float $time
   * @param string $easing
   */
  void ease_speed(float speed, float time, const std::string& easing);

  /**
   * @scripting
   * @description Sets the RGBA color of the spotlight.
   * @param float $r
   * @param float $g
   * @param float $b
   * @param float $a
   */
  inline void set_color_rgba(float r, float g, float b, float a) { m_color = Color(r, g, b, a); }
  /**
   * @scripting
   * @description Fades the spotlight to a new RGBA color in ""time"" seconds.
   * @param float $r
   * @param float $g
   * @param float $b
   * @param float $a
   * @param float $time
   */
  inline void fade_color_rgba(float r, float g, float b, float a, float time) { ease_color(time, Color(r, g, b, a)); }
  /**
   * @scripting
   * @description Fades the spotlight to a new RGBA color in ""time"" seconds, with easing.
   * @param float $r
   * @param float $g
   * @param float $b
   * @param float $a
   * @param float $time
   * @param string $easing
   */
  void ease_color_rgba(float r, float g, float b, float a, float time, const std::string& easing);

  void ease_angle(float time, float target, EasingMode ease = EasingMode::EaseNone);
  void ease_speed(float time, float target, EasingMode ease = EasingMode::EaseNone);
  void ease_color(float time, Color target, EasingMode ease = EasingMode::EaseNone);

private:
  /**
   * @scripting
   * @description The angle of the spotlight.
   */
  float m_angle;
  SpritePtr m_center;
  SpritePtr m_base;
  SpritePtr m_lights;
  SpritePtr m_light;
  SpritePtr m_lightcone;

  Color m_color;

  /**
   * @scripting
   * @description Speed that the spotlight is rotating with.
   */
  float m_speed;

  /** The direction of the spotlight */
  Direction m_direction;

  /** The layer (z-pos) of the spotlight. */
  int m_layer;

  /**
   * @scripting
   * @description Determines whether the spotlight is enabled.
   */
  bool m_enabled;

private:
  Spotlight(const Spotlight&) = delete;
  Spotlight& operator=(const Spotlight&) = delete;
};

#endif

/* EOF */
