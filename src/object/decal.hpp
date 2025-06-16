//  SuperTux - Decal
//  Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_DECAL_HPP
#define HEADER_SUPERTUX_OBJECT_DECAL_HPP

#include "object/moving_sprite.hpp"
#include "supertux/timer.hpp"

class ReaderMapping;

/**
 * A decorative image, perhaps part of the terrain.

 * @scripting
 * @summary A ""Decal"" that was given a name can be controlled by scripts.
 * @instances A ""Decal"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class Decal final : public MovingSprite
{
  friend class FlipLevelTransformer;

public:
  static void register_class(ssq::VM& vm);

public:
  Decal(const ReaderMapping& reader);
  ~Decal() override;

  virtual HitResponse collision(MovingObject& , const CollisionHit& ) override { return FORCE_MOVE; }

  static std::string class_name() { return "decal"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Decal"; }
  static std::string display_name() { return _("Decal"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(Decal)); }

  virtual ObjectSettings get_settings() override;

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;

  virtual void on_flip(float height) override;

  /**
   * @scripting
   * @description Fades the decal sprite to a new one in ""time"" seconds.
   * @param string $sprite
   * @param float $time
   */
  void fade_sprite(const std::string& sprite, float time);
#ifdef DOXYGEN_SCRIPTING
  /**
   * @scripting
   * @deprecated Use ""set_sprite()"" instead!
   * @description Changes the decal sprite.
   * @param string $sprite
   */
  void change_sprite(const std::string& sprite);
#endif
  /**
   * @scripting
   * @description Fades in the decal in ""time"" seconds.
   * @param float $time
   */
  void fade_in(float time);
  /**
   * @scripting
   * @description Fades out the decal in ""time"" seconds.
   * @param float $time
   */
  void fade_out(float time);

  inline void set_visible(bool v) { m_visible = v; }
  inline bool is_visible() const { return m_visible; }

private:
  std::string m_default_action;
  bool m_solid;
  SpritePtr m_fade_sprite;
  Timer m_fade_timer;
  Timer m_sprite_timer;
  bool m_visible;

private:
  Decal(const Decal&) = delete;
  Decal& operator=(const Decal&) = delete;
};

#endif

/* EOF */
