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

#ifndef HEADER_SUPERTUX_OBJECT_ROCK_HPP
#define HEADER_SUPERTUX_OBJECT_ROCK_HPP

#include "object/moving_sprite.hpp"
#include "object/portable.hpp"
#include "supertux/physic.hpp"

class Rock : public MovingSprite,
             public Portable
{
public:
  Rock(const ReaderMapping& reader, const std::string& spritename = "images/objects/rock/rock.sprite");
  Rock(const Vector& pos, const std::string& spritename = "images/objects/rock/rock.sprite");

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;

  virtual void grab(MovingObject& object, const Vector& pos, Direction dir) override;
  virtual void ungrab(MovingObject& object, Direction dir) override;

  static std::string class_name() { return "rock"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Rock"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(Portable)).add(typeid(Rock)); }

  virtual ObjectSettings get_settings() override;
  virtual GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;
  void draw(DrawingContext& context) override;

  /** Adds velocity from wind */
  virtual void add_wind_velocity(const Vector& velocity, const Vector& end_speed);
  inline Physic& get_physic() { return m_physic; }

private:
  enum Type {
    SMALL,
    LARGE
  };

protected:
  Physic m_physic;
  bool m_on_ground;
  bool m_on_ice;
  bool m_at_ceiling;
  Vector m_last_movement;
  std::string m_on_grab_script;
  std::string m_on_ungrab_script;
  bool m_running_grab_script;
  bool m_running_ungrab_script;
  float m_last_sector_gravity;

private:
  Rock(const Rock&) = delete;
  Rock& operator=(const Rock&) = delete;
};

#endif

/* EOF */
