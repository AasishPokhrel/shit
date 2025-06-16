//  Copyright (C) 2020 Daniel Ward <weluvgoatz@gmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_FALLBLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_FALLBLOCK_HPP

#include "object/moving_sprite.hpp"

#include "supertux/physic.hpp"
#include "supertux/timer.hpp"

class Player;

class FallBlock : public MovingSprite

{
public:
  FallBlock(const ReaderMapping& reader);

  virtual void update(float dt_sec) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void collision_solid(const CollisionHit& hit) override;

  virtual void draw(DrawingContext& context) override;

  static std::string class_name() { return "fallblock"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Falling Platform"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(FallBlock)); }

  virtual void on_flip(float height) override;

public:
  inline Physic& get_physic() { return m_physic; }

  enum State
  {
    IDLE,
    SHAKE,
    FALL,
    LAND
  };

  inline State get_state() const { return m_state; }

private:
  State m_state;

  Physic m_physic;
  Timer m_timer;

private:
  bool found_victim_down() const;

  FallBlock(const FallBlock&) = delete;
  FallBlock& operator=(const FallBlock&) = delete;
};

#endif

/* EOF */
