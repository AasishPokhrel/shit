//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_OBJECT_CAMERA_HPP
#define HEADER_SUPERTUX_OBJECT_CAMERA_HPP

#include "editor/layer_object.hpp"

#include <string>

#include "math/anchor_point.hpp"
#include "math/size.hpp"
#include "math/vector.hpp"
#include "object/path_object.hpp"
#include "supertux/timer.hpp"

class Path;
class PathWalker;
class ReaderMapping;

/**
 * @scripting
 * @summary A ""Camera"" that was given a name can be manipulated by scripts.
 * @instances An instance named ""Camera"" (""sector.Camera"" in the console) is available.${SRG_NEWPARAGRAPH}
              The mode of the camera is either ""normal"" (the camera is following the player) or ""autoscroll"". In the latter mode, the camera is forced along a specified path.
 */
class Camera final : public LayerObject,
                     public PathObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  enum class Mode
  {
    NORMAL, MANUAL, AUTOSCROLL, SCROLLTO
  };

public:
  Camera(const std::string& name);
  Camera(const ReaderMapping& reader);
  ~Camera() override;

  /** \addtogroup GameObject
      @{ */
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& ) override {}

  virtual bool is_singleton() const override { return true; }
  virtual bool is_saveable() const override;

  static std::string class_name() { return "camera"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Camera"; }
  static std::string display_name() { return _("Camera"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(PathObject)).add(typeid(Camera)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  void save_state() override;
  void check_state() override;

  virtual const std::string get_icon_path() const override { return "images/engine/editor/camera.png"; }
  /** @} */

  Rectf get_rect() const;

  /** \addtogroup CameraAPI
      @{ */

  /** reset camera position */
  void reset(const Vector& tuxpos);

  /** return camera position */
  const Vector get_translation() const;
  inline void set_translation(const Vector& translation) { m_translation = translation; }
  void set_translation_centered(const Vector& translation);

  void keep_in_bounds(const Rectf& bounds);

  /** scroll the upper left edge of the camera in scrolltime seconds
      to the position goal */
  void scroll_to(const Vector& goal, float scrolltime);
  void move(const Vector& offset);

  /** get the coordinates of the point directly in the center of this
      camera */
  Vector get_center() const;

  /** get the coordinates of the point directly in the top left of this
      camera */
  const Vector& get_position() const;

  /** get the width and height of the screen*/
  const Sizef& get_screen_size() const;

  inline void set_mode(Mode mode_) { m_mode = mode_; }

  inline Mode get_mode() const { return m_mode; }

  /** smoothly slide the scale and anchor position of the camera towards a new value */
  void ease_scale(float scale, float time, easing ease, AnchorPoint anchor = AnchorPoint::ANCHOR_MIDDLE);
  /** @} */

  /**
   * @scripting
   * @description Shakes the camera in a certain direction only 1 time.
   * @param float $duration
   * @param float $x
   * @param float $y
   */
  void shake(float duration, float x, float y);
  /**
   * @scripting
   * @description Starts "earthquake" mode, which shakes the camera vertically with a specified average ""strength"", at a certain minimal ""delay"", until stopped.
   * @param float $strength
   * @param float $delay
   */
  void start_earthquake(float strength, float delay);
  /**
   * @scripting
   * @description Stops "earthquake" mode.
   */
  void stop_earthquake();
  /**
   * @scripting
   * @description Moves the camera to the specified absolute position. The origin is at the top left.
   * @param float $x
   * @param float $y
   */
  inline void set_pos(float x, float y) { scroll_to(Vector(x, y), 0.0f); }
  /**
   * @scripting
   * @description Moves the camera ""x"" to the left and ""y"" down.
   * @param float $x
   * @param float $y
   */
  void move(float x, float y);
  /**
   * @scripting
   * @description Sets the camera mode.
   * @param string $mode The mode can be "normal" or "manual".
   */
  void set_mode(const std::string& mode);
  /**
   * @scripting
   * @description Scrolls the camera to specific coordinates in ""scrolltime"" seconds.
   * @param float $x
   * @param float $y
   * @param float $scrolltime
   */
  inline void scroll_to(float x, float y, float scrolltime) { scroll_to(Vector(x, y), scrolltime); }
  /**
   * @scripting
   * @description Returns the current scale factor of the camera.
   */
  float get_current_scale() const;
  /**
   * @scripting
   * @description Returns the scale factor the camera is fading towards.
   */
  inline float get_target_scale() const { return m_scale_target; }
  /**
   * @scripting
   * @description Fades to a specified scale factor and target position anchor in ""time"" seconds with easing (smooth movement).
                  NOTE: Target position anchor is only applied, if the camera is in "manual" mode.
   * @param float $scale
   * @param float $time
   * @param int $anchor Anchor point as represented by the ""ANCHOR_*"" constants.
                        Optional, default is ""ANCHOR_MIDDLE"" (see ${SRG_REF_AnchorPoints}).
   * @param string $ease Optional, empty by default.
   */
  void set_scale(float scale, float time = 0.f, int anchor = ANCHOR_MIDDLE, const std::string& ease = "");
  /**
   * @scripting
   * @deprecated Use ""set_scale()"" instead!
   * @description Sets the scale factor and the target position anchor.
                  NOTE: Target position anchor is only applied, if the camera is in "manual" mode.
   * @param float $scale
   * @param int $anchor Anchor point as represented by the ""ANCHOR_*"" constants (see ${SRG_REF_AnchorPoints}).
   */
  inline void set_scale_anchor(float scale, int anchor) { set_scale(scale, 0, anchor, ""); }
  /**
   * @scripting
   * @deprecated Use ""set_scale()"" instead!
   * @description Fades to a specified scale factor in ""time"" seconds.
   * @param float $scale
   * @param float $time
   */
  inline void scale(float scale, float time) { set_scale(scale, time, AnchorPoint::ANCHOR_MIDDLE, ""); }
  /**
   * @scripting
   * @deprecated Use ""set_scale()"" instead!
   * @description Fades to a specified scale factor and target position anchor in ""time"" seconds.
                  NOTE: Target position anchor is only applied, if the camera is in "manual" mode.
   * @param float $scale
   * @param float $time
   * @param int $anchor Anchor point as represented by the ""ANCHOR_*"" constants (see ${SRG_REF_AnchorPoints}).
   */
  inline void scale_anchor(float scale, float time, int anchor) { set_scale(scale, time, anchor, ""); }
  /**
   * @scripting
   * @deprecated Use ""set_scale()"" instead!
   * @description Fades to a specified scale factor in ""time"" seconds with easing (smooth movement).
   * @param float $scale
   * @param float $time
   * @param string $ease
   */
  inline void ease_scale(float scale, float time, const std::string& ease) { set_scale(scale, time, AnchorPoint::ANCHOR_MIDDLE, ease); }
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @deprecated Use ""set_scale()"" instead!
   * @description Fades to a specified scale factor and target position anchor in ""time"" seconds with easing (smooth movement).
                  NOTE: Target position anchor is only applied, if the camera is in "manual" mode.
   * @param float $scale
   * @param float $time
   * @param int $anchor Anchor point as represented by the ""ANCHOR_*"" constants (see ${SRG_REF_AnchorPoints}).
   * @param string $ease
   */
  void ease_scale_anchor(float scale, float time, int anchor, const std::string& ease);
#endif
  /**
   * @scripting
   * @description Gets the current width of the screen.
   */
  inline float get_screen_width() const { return m_screen_size.width; }
  /**
   * @scripting
   * @description Gets the current height of the screen.
   */
  inline float get_screen_height() const { return m_screen_size.height; }
  /**
   * @scripting
   * @description Gets the X coordinate of the top-left corner of the screen.
   */
  inline float get_x() const { return m_translation.x; }
  /**
   * @scripting
   * @description Gets the Y coordinate of the top-left corner of the screen.
   */
  inline float get_y() const { return m_translation.y; }

private:
  void keep_in_bounds(Vector& vector);

  void update_scroll_normal(float dt_sec);
  void update_scroll_normal_multiplayer(float dt_sec);
  void update_scroll_autoscroll(float dt_sec);
  void update_scroll_to(float dt_sec);
  void update_scale(float dt_sec);
  void update_shake();
  void update_earthquake();

  Vector get_scale_anchor_target() const;
  void reload_scale();

private:
  Mode m_mode;
  Mode m_defaultmode;

  Sizef m_screen_size;

  Vector m_translation;

  // normal mode
  Vector m_lookahead_pos;
  Vector m_peek_pos;
  Vector m_cached_translation;

  // shaking
  Timer m_shaketimer;
  float m_shakespeed;
  float m_shakedepth_x;
  float m_shakedepth_y;

  // Earthquake
  bool m_earthquake;
  float m_earthquake_strength,
        m_earthquake_delay,
        m_earthquake_last_offset;
  Timer m_earthquake_delay_timer;

  // scrollto mode
  Vector m_scroll_from;
  Vector m_scroll_goal;
  float m_scroll_to_pos;
  float m_scrollspeed;

  float m_scale,
        m_scale_origin,
        m_scale_target,
        m_scale_time_total,
        m_scale_time_remaining;
  Vector m_scale_origin_translation,
         m_scale_target_translation;
  easing m_scale_easing;
  AnchorPoint m_scale_anchor;

  // Minimum scale is used in certain circumstances where a fixed minimum scale
  // should be used, regardless of the scriping-accessible `m_scale` property.
  float m_minimum_scale;
  bool m_enfore_minimum_scale;

private:
  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;
};

#endif

/* EOF */
