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

#ifndef HEADER_SUPERTUX_OBJECT_LEVEL_TIME_HPP
#define HEADER_SUPERTUX_OBJECT_LEVEL_TIME_HPP

#include "editor/layer_object.hpp"

#include "video/color.hpp"
#include "video/surface_ptr.hpp"

class ReaderMapping;

/**
 * @scripting
 * @summary A ""LevelTime"" that was given a name can be controlled by scripts.
 * @instances A ""LevelTime"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class LevelTime final : public LayerObject
{
  static Color text_color;

public:
  static void register_class(ssq::VM& vm);

public:
  LevelTime(const ReaderMapping& reader);

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  /** @name Scriptable Methods
      @{ */

  /**
   * @scripting
   * @description Resumes the countdown (assuming it isn't already started, in which case it does nothing).
   */
  void start();
  /**
   * @scripting
   * @description Pauses the countdown (assuming it isn't already stopped, in which case it does nothing).
   */
  void stop();
  /**
   * @scripting
   * @description Returns the number of seconds left on the clock.
   */
  float get_time() const;
  /**
   * @scripting
   * @description Sets the number of seconds left on the clock.
   * @param float $time_left
   */
  void set_time(float time_left);

  /** @} */

  static std::string class_name() { return "leveltime"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "LevelTime"; }
  static std::string display_name() { return _("Time Limit"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(LevelTime)); }

  virtual ObjectSettings get_settings() override;

  virtual const std::string get_icon_path() const override { return "images/engine/editor/clock.png"; }

private:
  SurfacePtr time_surface;
  bool running;
  float time_left;

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description The number of seconds left on the clock.
   */
  float m_time;
#endif

private:
  LevelTime(const LevelTime&) = delete;
  LevelTime& operator=(const LevelTime&) = delete;
};

#endif

/* EOF */
