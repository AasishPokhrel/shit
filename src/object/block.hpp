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

#ifndef HEADER_SUPERTUX_OBJECT_BLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_BLOCK_HPP

#include "object/moving_sprite.hpp"

class Player;
class ReaderMapping;

class Block : public MovingSprite
{
  friend class FlipLevelTransformer;

public:
  Block(const Vector& pos, const std::string& sprite_file);
  Block(const ReaderMapping& mapping, const std::string& sprite_file);

  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(Block)); }

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;

  virtual void on_flip(float height) override;

  void start_bounce(MovingObject* hitter);

protected:
  virtual void hit(Player& player) = 0;

  void start_break(MovingObject* hitter);
  void break_me();

protected:
  bool m_bouncing;
  bool m_breaking;
  float m_bounce_dir;
  float m_bounce_offset;
  float m_original_y;

private:
  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;
};

#endif

/* EOF */
