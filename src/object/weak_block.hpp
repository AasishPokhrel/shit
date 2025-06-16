//  SuperTux - Weak Block
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_WEAK_BLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_WEAK_BLOCK_HPP

#include "object/moving_sprite.hpp"

class Bullet;

/** A block that can be destroyed by Bullet hits */
class WeakBlock final : public MovingSprite
{
public:
  WeakBlock(const ReaderMapping& mapping);

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  static std::string class_name() { return "weak_block"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Weak Tile"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(WeakBlock)); }

  std::vector<std::string> get_patches() const override;
  void update_version() override;
  void save(Writer& writer) override;

  GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

  virtual void on_flip(float height) override;

  void startBurning();

private:
  virtual HitResponse collision_bullet(Bullet& bullet, const CollisionHit& hit);

private:
  /** called by self when hit by a bullet */

  /** pass hit to nearby WeakBlock objects */
  void spreadHit();

private:
  enum Type {
    ICE,
    HAY
  };

  enum State {
    STATE_NORMAL, /**< default state */
    STATE_BURNING, /**< on fire, still solid */
    STATE_DISINTEGRATING /**< crumbling to dust, no longer solid */
  };

private:
  State state;
  SpritePtr lightsprite;

private:
  WeakBlock(const WeakBlock&) = delete;
  WeakBlock& operator=(const WeakBlock&) = delete;
};

#endif

/* EOF */
