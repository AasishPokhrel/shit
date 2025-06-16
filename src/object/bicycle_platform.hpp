//  SuperTux - BicyclePlatform
//  Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_BICYCLE_PLATFORM_HPP
#define HEADER_SUPERTUX_OBJECT_BICYCLE_PLATFORM_HPP

#include "object/path_walker.hpp"
#include "object/moving_sprite.hpp"

class BicyclePlatform;

class BicyclePlatformChild : public MovingSprite
{
  friend class BicyclePlatform;

public:
  BicyclePlatformChild(const ReaderMapping& reader, float angle_offset, BicyclePlatform& parent);
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(BicyclePlatformChild)); }

  virtual void update(float dt_sec) override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual bool is_saveable() const override { return false; }
  virtual void on_flip(float height) override;

  virtual void editor_delete() override;

private:
  BicyclePlatform& m_parent;
  float m_angle_offset;
  float m_momentum; /** angular momentum in rad per second per second*/
  std::set<GameObject*> m_contacts; /**< objects that are currently pushing on the platform */

private:
  BicyclePlatformChild(const BicyclePlatformChild&) = delete;
  BicyclePlatformChild& operator=(const BicyclePlatformChild&) = delete;
};

/**
 * Used to construct a pair of bicycle platforms: If one is pushed down, the other one rises
 */
class BicyclePlatform final : public GameObject
{
  friend class BicyclePlatformChild;

public:
  BicyclePlatform(const ReaderMapping& reader);
  ~BicyclePlatform() override;

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual void on_flip(float height) override;

  static std::string class_name() { return "bicycle-platform"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Bicycle Platform"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(BicyclePlatform)); }

  virtual ObjectSettings get_settings() override;
  virtual void editor_delete() override;
  virtual void after_editor_set() override;

private:
  Vector m_center; /**< pivot point */
  float m_radius; /**< radius of circle */

  float m_angle; /**< current angle */
  float m_angular_speed; /**< angular speed in rad per second */

  float m_momentum_change_rate; /** Change in momentum every step **/

  std::vector<BicyclePlatformChild*> m_children;
  std::unique_ptr<PathWalker> m_walker;
  int m_platforms;

private:
  BicyclePlatform(const BicyclePlatform&) = delete;
  BicyclePlatform& operator=(const BicyclePlatform&) = delete;
};

#endif

/* EOF */
