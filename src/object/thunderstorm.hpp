//  SuperTux - Thunderstorm Game Object
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

#ifndef HEADER_SUPERTUX_OBJECT_THUNDERSTORM_HPP
#define HEADER_SUPERTUX_OBJECT_THUNDERSTORM_HPP

#include "editor/layer_object.hpp"

#include <list>
#include <map>

#include "supertux/timer.hpp"

class DrawingContext;
class ReaderMapping;

/**
 * Thunderstorm scriptable GameObject: Plays thunder, lightning and
   electrifies water at regular interval.

 * @scripting
 * @summary A ""Thunderstorm"" that was given a name can be controlled by scripts.
 * @instances A ""Thunderstorm"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class Thunderstorm final : public LayerObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  Thunderstorm(const ReaderMapping& reader);

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  static std::string class_name() { return "thunderstorm"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Thunderstorm"; }
  static std::string display_name() { return _("Thunderstorm"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(Thunderstorm)); }

  virtual ObjectSettings get_settings() override;

  virtual const std::string get_icon_path() const override { return "images/engine/editor/thunderstorm.png"; }

  /** @name Scriptable Methods
      @{ */

  /**
   * @scripting
   * @description Starts playing thunder and lightning at a configured interval.
   */
  void start();
  /**
   * @scripting
   * @description Stops playing thunder and lightning at a configured interval.
   */
  void stop();

  /**
   * @scripting
   * @description Plays thunder.
   */
  void thunder();
  /**
   * @scripting
   * @description Plays lightning, i.e. calls ""flash()"" and ""electrify()"".
   */
  void lightning();

  /**
   * @scripting
   * @description Displays a flash.
   */
  void flash();
  /**
   * @scripting
   * @description Electrifies water throughout the whole sector for a short time.
   */
  void electrify();

  /** @} */

private:
  void lightning_general(bool is_scripted = false);

  void change_background_colors(bool is_lightning, bool is_scripted = false);
  void restore_background_colors();

private:
  bool running; /**< whether we currently automatically trigger lightnings */
  float interval; /**< time between two lightnings */
  int layer; /**< layer, where flash will be painted */

  std::string m_strike_script;

  Timer time_to_thunder; /**< counts down until next thunder */
  Timer time_to_lightning; /**< counts down until next lightning */
  Timer flash_display_timer; /**< counts down while flash is displayed */
  Timer restore_background_color_timer; /*Helps return the background color to normal */
  std::list<Color> m_background_colors; /*Helps return the background color to normal */

  std::map<uint32_t, uint32_t> changing_tiles; /**< preserves the tiles which an electrocution should change */
  Color m_flash_color;

private:
  Thunderstorm(const Thunderstorm&) = delete;
  Thunderstorm& operator=(const Thunderstorm&) = delete;
};

#endif

/* EOF */
