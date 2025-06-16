//  SuperTux - PushButton running a script
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_OBJECT_PUSHBUTTON_HPP
#define HEADER_SUPERTUX_OBJECT_PUSHBUTTON_HPP

#include "object/sticky_object.hpp"

/** PushButton - jump on it to run a script */
class PushButton final : public StickyObject
{
public:
  PushButton(const ReaderMapping& reader);

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;
  static std::string class_name() { return "pushbutton"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Button"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return StickyObject::get_class_types().add(typeid(PushButton)); }

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

  virtual void on_flip(float height) override;

private:
  enum PushButtonState {
    OFF,
    ON
  };

  std::string m_script;
  PushButtonState m_state;

  Direction m_dir;

private:
  PushButton(const PushButton&) = delete;
  PushButton& operator=(const PushButton&) = delete;
};

#endif

/* EOF */
