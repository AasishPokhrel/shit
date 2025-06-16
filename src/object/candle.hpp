//  SuperTux
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

#ifndef HEADER_SUPERTUX_OBJECT_CANDLE_HPP
#define HEADER_SUPERTUX_OBJECT_CANDLE_HPP

#include "object/moving_sprite.hpp"

/**
 * A burning candle: Simple, scriptable level decoration.

 * @scripting
 * @summary A ""Candle"" that was given a name can be controlled by scripts.
 * @instances A ""Candle"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class Candle final : public MovingSprite
{
public:
  static void register_class(ssq::VM& vm);

public:
  Candle(const ReaderMapping& mapping);
  virtual void draw(DrawingContext& context) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  static std::string class_name() { return "candle"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Candle"; }
  static std::string display_name() { return _("Candle"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(Candle)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  virtual void on_flip(float height) override;

  /**
   * @scripting
   * @description Spawns a puff of smoke.
   */
  void puff_smoke();
  /**
   * @scripting
   * @description Returns ""true"" if the candle is lit up.
   */
  inline bool get_burning() const { return burning; }
  /**
   * @scripting
   * @description Sets the burning state of the candle.
   * @param bool $burning If ""true"", the candle is lit up. If ""false"", it's extinguished.
   */
  void set_burning(bool burning);

private:
  /**
   * @scripting
   * @description The burning state of the candle.
   */
  bool burning; /**< true if candle is currently lighted */
  bool flicker; /**< true if candle light is to flicker */
  Color lightcolor; /**< determines color or light given off */
  SpritePtr candle_light_1; /**< drawn to lightmap */
  SpritePtr candle_light_2; /**< drawn to lightmap (alternative image) */

private:
  Candle(const Candle&) = delete;
  Candle& operator=(const Candle&) = delete;
};

#endif

/* EOF */
