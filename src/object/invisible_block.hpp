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

#ifndef HEADER_SUPERTUX_OBJECT_INVISIBLE_BLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_INVISIBLE_BLOCK_HPP

#include "object/block.hpp"

class InvisibleBlock final : public Block
{
public:
  InvisibleBlock(const Vector& pos);
  InvisibleBlock(const ReaderMapping& mapping);

  static std::string class_name() { return "invisible_block"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Invisible Block"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Block::get_class_types().add(typeid(InvisibleBlock)); }

  virtual void draw(DrawingContext& context) override;
  virtual bool collides(MovingObject& other, const CollisionHit& hit) const override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

private:
  virtual void hit(Player& player) override;

private:
  enum Type {
    NORMAL,
    RETRO
  };

private:
  bool visible;

private:
  InvisibleBlock(const InvisibleBlock&) = delete;
  InvisibleBlock& operator=(const InvisibleBlock&) = delete;
};

#endif

/* EOF */
