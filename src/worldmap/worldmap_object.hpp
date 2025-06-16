//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_WORLDMAP_WORLDMAP_OBJECT_HPP
#define HEADER_SUPERTUX_WORLDMAP_WORLDMAP_OBJECT_HPP

#include "object/moving_sprite.hpp"

#include "math/vector.hpp"

class DrawingContext;
class ReaderMapping;

namespace worldmap {

class WorldMapObject : public MovingSprite
{
protected:
  static bool in_worldmap();

public:
  WorldMapObject(const ReaderMapping& mapping, const std::string& default_sprite);
  WorldMapObject(const ReaderMapping& mapping);
  WorldMapObject(const Vector& pos, const std::string& default_sprite);
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(WorldMapObject)); }

  static std::string class_name() { return "worldmap-object"; }
  virtual std::string get_class_name() const override { return class_name(); }

  void draw(DrawingContext& context) override;

  /** Draws the object, when on a worldmap. */
  virtual void draw_worldmap(DrawingContext& context);

  void update(float) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override { return FORCE_MOVE; }
  virtual ObjectSettings get_settings() override;
  virtual void move_to(const Vector& pos) override;

  inline Vector get_tile_pos() const { return { m_tile_x, m_tile_y }; }

private:
  void initialize();

  void draw_normal(DrawingContext& context);
  void update_pos();

private:
  int m_tile_x;
  int m_tile_y;

private:
  WorldMapObject(const WorldMapObject&) = delete;
  WorldMapObject& operator=(const WorldMapObject&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
