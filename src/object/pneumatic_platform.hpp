//  SuperTux - PneumaticPlatform
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

#ifndef HEADER_SUPERTUX_OBJECT_PNEUMATIC_PLATFORM_HPP
#define HEADER_SUPERTUX_OBJECT_PNEUMATIC_PLATFORM_HPP

#include "object/moving_sprite.hpp"

class PneumaticPlatform;

class PneumaticPlatformChild final : public MovingSprite
{
  friend class PneumaticPlatform;

public:
  PneumaticPlatformChild(const ReaderMapping& reader, bool left, PneumaticPlatform& parent);
  ~PneumaticPlatformChild() override;
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(PneumaticPlatformChild)); }

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;
  virtual bool is_saveable() const override { return false; }
  virtual void on_flip(float height) override;

  virtual void editor_delete() override;

protected:
  PneumaticPlatform& m_parent;
  bool m_left;
  std::set<GameObject*> m_contacts; /**< objects that are currently pushing on the platform */

private:
  PneumaticPlatformChild(const PneumaticPlatformChild&) = delete;
  PneumaticPlatformChild& operator=(const PneumaticPlatformChild&) = delete;
};

/** Used to construct a pair of pneumatic platforms: If one is pushed
    down, the other one rises */
class PneumaticPlatform final : public GameObject
{
  friend class PneumaticPlatformChild;

public:
  PneumaticPlatform(const ReaderMapping& mapping);
  ~PneumaticPlatform() override;

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual void on_flip(float height) override;

  static std::string class_name() { return "pneumatic-platform"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Pneumatic Platform"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(PneumaticPlatform)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;
  virtual void editor_delete() override;

private:
  Vector m_pos;
  std::string m_sprite_name;
  float m_start_y; /**< vertical start position */
  float m_speed_y; /**< vertical speed */
  float m_offset_y; /**< vertical offset from the start position in px */
  std::vector<PneumaticPlatformChild*> m_children;

private:
  PneumaticPlatform(const PneumaticPlatform&) = delete;
  PneumaticPlatform& operator=(const PneumaticPlatform&) = delete;
};

#endif

/* EOF */
