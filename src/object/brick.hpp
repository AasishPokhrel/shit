//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_BRICK_HPP
#define HEADER_SUPERTUX_OBJECT_BRICK_HPP

#include "object/block.hpp"

class Crusher;

class Brick : public Block
{
public:
  Brick(const Vector& pos, int data, const std::string& sprite_name);
  Brick(const ReaderMapping& mapping, const std::string& sprite_name = "images/objects/bonus_block/brick.sprite");

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual ObjectSettings get_settings() override;
  static std::string class_name() { return "brick"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Brick"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Block::get_class_types().add(typeid(Brick)); }

  GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

  void try_break(Player* player, bool slider = false);
  void break_for_crusher(Crusher* crusher);

protected:
  virtual void hit(Player& player) override;

private:
  enum Type {
    NORMAL,
    RETRO
  };

private:
  bool m_breakable;
  int m_coin_counter;

private:
  Brick(const Brick&) = delete;
  Brick& operator=(const Brick&) = delete;
};

class HeavyBrick : public Brick
{
public:
  HeavyBrick(const Vector& pos, int data, const std::string& spriteName);
  HeavyBrick(const ReaderMapping& mapping);

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  static std::string class_name() { return "heavy-brick"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Heavy Brick"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Brick::get_class_types().add(typeid(HeavyBrick)); }

  GameObjectTypes get_types() const override { return {}; }

protected:
  virtual void hit(Player& player) override;

private:
  void ricochet(MovingObject* collider);

private:
  HeavyBrick(const HeavyBrick&) = delete;
  HeavyBrick& operator=(const HeavyBrick&) = delete;
};

#endif

/* EOF */
