//  SuperTux - MagicBlock
//
//  Magic Blocks are tile-like game objects that are sensitive to
//  lighting conditions. They are rendered in a color and
//  will only be solid as long as light of the same color shines
//  on the block. The black block becomes solid, if any kind of
//  light is above MIN_INTENSITY.
//
//  Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_MAGICBLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_MAGICBLOCK_HPP

#include "object/moving_sprite.hpp"

#include <memory>

class MagicBlock final: public MovingSprite
{
public:
  MagicBlock(const ReaderMapping& reader);

  virtual bool collides(MovingObject& other, const CollisionHit& hit) const override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  static std::string class_name() { return "magicblock"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Magic Tile"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(MagicBlock)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  virtual void on_flip(float height) override;

private:
  void set_trigger_color();

private:
  bool m_is_solid;
  float m_trigger_red;
  float m_trigger_green;
  float m_trigger_blue;
  float m_solid_time;
  float m_switch_delay; /**< seconds until switching solidity */
  Rectf m_solid_box;
  Color m_color;
  std::shared_ptr<Color> m_light;
  Vector m_center;
  bool m_black;

private:
  MagicBlock(const MagicBlock&) = delete;
  MagicBlock& operator=(const MagicBlock&) = delete;
};

#endif

/* EOF */
