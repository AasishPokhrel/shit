//  SuperTux
//  Copyright (C) 2022 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_LIT_OBJECT_HPP
#define HEADER_SUPERTUX_OBJECT_LIT_OBJECT_HPP

#include "object/moving_sprite.hpp"

class ReaderMapping;

/**
 * @scripting
 * @summary A ""LitObject"" that was given a name can be controlled by scripts.
 * @instances A ""LitObject"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class LitObject final : public MovingSprite
{
public:
  static void register_class(ssq::VM& vm);

public:
  LitObject(const ReaderMapping& reader);

  virtual void draw(DrawingContext& context) override;
  virtual void update(float) override;

  virtual HitResponse collision(MovingObject&, const CollisionHit&) override { return ABORT_MOVE; }

  static std::string class_name() { return "lit-object"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "LitObject"; }
  static std::string display_name() { return _("Lit object"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(LitObject)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  virtual void on_flip(float height) override;

  /**
   * @scripting
   * @description Returns the current light sprite action.
   */
  const std::string& get_light_action() const;
  /**
   * @scripting
   * @description Sets the light sprite action.
   * @param string $action
   */
  void set_light_action(const std::string& action);

private:
  Vector m_light_offset;
  std::string m_light_sprite_name;
  std::string m_sprite_action;
  std::string m_light_sprite_action;
  SpritePtr m_light_sprite;

#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @description The current light sprite action.
   */
  std::string m_light_action;
#endif

private:
  LitObject(const LitObject&) = delete;
  LitObject& operator=(const LitObject&) = delete;
};

#endif

/* EOF */
