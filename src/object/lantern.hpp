//  SuperTux - Lantern
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

#ifndef HEADER_SUPERTUX_OBJECT_LANTERN_HPP
#define HEADER_SUPERTUX_OBJECT_LANTERN_HPP

#include "object/rock.hpp"

/** Lantern. A portable Light Source. */
class Lantern final : public Rock
{
public:
  Lantern(const Vector& pos);
  Lantern(const ReaderMapping& reader);

  virtual void draw(DrawingContext& context) override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  virtual void grab(MovingObject& object, const Vector& pos, Direction dir) override;
  virtual void ungrab(MovingObject& object, Direction dir) override;

  static std::string class_name() { return "lantern"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Lantern"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Rock::get_class_types().add(typeid(Lantern)); }

  virtual ObjectSettings get_settings() override;
  virtual GameObjectTypes get_types() const override { return {}; }
  virtual void after_editor_set() override;

  /** returns true if lamp is currently open */
  bool is_open() const;

  /** returns the lamp's color */
  inline Color get_color() const { return lightcolor; }
  void add_color(const Color& c);

private:
  Color lightcolor;
  SpritePtr lightsprite;
  void updateColor();

private:
  Lantern(const Lantern&) = delete;
  Lantern& operator=(const Lantern&) = delete;
};

#endif

/* EOF */
