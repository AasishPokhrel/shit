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

#ifndef HEADER_SUPERTUX_OBJECT_FIREFLY_HPP
#define HEADER_SUPERTUX_OBJECT_FIREFLY_HPP

#include "object/moving_sprite.hpp"
#include "sprite/sprite_ptr.hpp"

/**
 * A Firefly: When tux touches it, it begins buzzing and you will respawn at this
 * position.
 */
class Firefly final : public MovingSprite
{
public:
  Firefly(const ReaderMapping& mapping);

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  static std::string class_name() { return "firefly"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Checkpoint"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(Firefly)); }
  virtual ObjectSettings get_settings() override;

  virtual void on_flip(float height) override;

private:
  void update_state();

private:
  SpritePtr m_sprite_light;
  bool activated;
  Vector initial_position; /**< position as in level file. This is where Tux will have to respawn, as the level is reset every time */

private:
  Firefly(const Firefly&) = delete;
  Firefly& operator=(const Firefly&) = delete;
};

#endif

/* EOF */
