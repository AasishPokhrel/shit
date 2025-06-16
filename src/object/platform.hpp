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

#ifndef HEADER_SUPERTUX_OBJECT_PLATFORM_HPP
#define HEADER_SUPERTUX_OBJECT_PLATFORM_HPP

#include "object/moving_sprite.hpp"
#include "object/path_object.hpp"

/**
 * This is the base class for platforms that Tux can stand on.

 * @scripting
 * @summary A ""Platform"" that was given a name can be controlled by scripts.
            It moves along a specified path.
 * @instances A ""Platform"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
*/
class Platform : public MovingSprite,
                 public PathObject
{
public:
  static void register_class(ssq::VM& vm);

public:
  Platform(const ReaderMapping& reader);
  Platform(const ReaderMapping& reader, const std::string& default_sprite);

  virtual void finish_construction() override;

  virtual ObjectSettings get_settings() override;

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;

  virtual void move_to(const Vector& pos) override;

  static std::string class_name() { return "platform"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "Platform"; }
  static std::string display_name() { return _("Platform"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(PathObject)).add(typeid(Platform)); }

  virtual void editor_update() override;

  virtual void on_flip(float height) override;

  void save_state() override;
  void check_state() override;

  inline const Vector& get_speed() const { return m_speed; }
  inline const Vector& get_movement() const { return m_movement; }

  /** Moves platform instantly to given node.
      Replaces PathObject::set_node's implementation in scripting. */
  void jump_to_node(int node_idx);

private:
  Vector m_speed;
  Vector m_movement;

  /** true if Platform will automatically pick a destination based on
      collisions and current Player position */
  bool m_automatic;

  /** true if a Player touched the Platform during the last round of
      collision detections */
  bool m_player_contact;

  /** true if a Player touched the Platform during the round before
      the last round of collision detections */
  bool m_last_player_contact;

  int m_starting_node;

private:
  Platform(const Platform&) = delete;
  Platform& operator=(const Platform&) = delete;
};

#endif

/* EOF */
