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

#ifndef HEADER_SUPERTUX_OBJECT_SCRIPTED_OBJECT_HPP
#define HEADER_SUPERTUX_OBJECT_SCRIPTED_OBJECT_HPP

#include "object/moving_sprite.hpp"
#include "supertux/physic.hpp"

/**
 * @scripting
 * @summary A ""ScriptedObject"" that was given a name can be controlled by scripts.
 * @instances A ""ScriptedObject"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class ScriptedObject final : public MovingSprite
{
public:
  static void register_class(ssq::VM& vm);

public:
  ScriptedObject(const ReaderMapping& mapping);

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  static std::string class_name() { return "scriptedobject"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "ScriptedObject"; }
  static std::string display_name() { return _("Scripted Object"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(ScriptedObject)); }

  virtual ObjectSettings get_settings() override;

  virtual void on_flip(float height) override;

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @deprecated Use ""get_x()"" instead!
   * @description Returns the X coordinate of the object's position.
   */
  float get_pos_x() const;
  /**
   * @scripting
   * @deprecated Use ""get_y()"" instead!
   * @description Returns the Y coordinate of the object's position.
   */
  float get_pos_y() const;
#endif

  /**
   * @scripting
   * @description Makes the object move in a certain ""x"" and ""y"" direction (with a certain speed).
   * @param float $x
   * @param float $y
   */
  void set_velocity(float x, float y);
  /**
   * @scripting
   * @description Returns the X coordinate of the object's velocity.
   */
  inline float get_velocity_x() const { return physic.get_velocity_x(); }
  /**
   * @scripting
   * @description Returns the Y coordinate of the object's velocity.
   */
  inline float get_velocity_y() const { return physic.get_velocity_y(); }

  /**
   * @scripting
   * @description Enables or disables gravity, according to the value of ""enabled"".
   * @param bool $enabled
   */
  inline void enable_gravity(bool enabled) { physic.enable_gravity(enabled); }
  /**
   * @scripting
   * @description Returns ""true"" if the object's gravity is enabled.
   */
  inline bool gravity_enabled() const { return physic.gravity_enabled(); }

  /**
   * @scripting
   * @description Shows or hides the object, according to the value of ""visible"".
   * @param bool $visible
   */
  inline void set_visible(bool visible_) { visible = visible_; }
  /**
   * @scripting
   * @description Returns ""true"" if the object is visible.
   */
  inline bool is_visible() const { return visible; }

  /**
   * @scripting
   * @description Changes the solidity, according to the value of ""solid"".
   * @param bool $solid
   */
  void set_solid(bool solid);
  /**
   * @scripting
   * @description Returns ""true"" if the object is solid.
   */
  inline bool is_solid() const { return solid; }

private:
  Physic physic;
  /**
   * @scripting
   * @description Determines whether the object is solid.
   */
  bool solid;
  bool physic_enabled;
  /**
   * @scripting
   * @description Determines whether the object is visible.
   */
  bool visible;
  std::string hit_script;
  bool new_vel_set;
  Vector new_vel;
  Vector new_size;

private:
  ScriptedObject(const ScriptedObject&) = delete;
  ScriptedObject& operator=(const ScriptedObject&) = delete;
};

#endif

/* EOF */
