//  SuperTux - Switch Trigger
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

#ifndef HEADER_SUPERTUX_TRIGGER_SWITCH_HPP
#define HEADER_SUPERTUX_TRIGGER_SWITCH_HPP

#include "trigger/trigger_base.hpp"

class Switch final : public StickyTrigger
{
public:
  Switch(const ReaderMapping& reader);
  ~Switch() override;

  static std::string class_name() { return "switch"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Switch"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return StickyTrigger::get_class_types().add(typeid(Switch)); }

  virtual ObjectSettings get_settings() override;

  virtual void update(float dt_sec) override;
  virtual void event(Player& player, EventType type) override;

  virtual void after_editor_set() override;
  virtual void on_flip(float height) override;

private:
  enum SwitchState {
    OFF,
    TURN_ON,
    ON,
    TURN_OFF
  };

private:
  std::string m_script;
  std::string m_off_script;
  SwitchState m_state;
  bool m_bistable;
  Direction m_dir;

private:
  Switch(const Switch&) = delete;
  Switch& operator=(const Switch&) = delete;
};

#endif

/* EOF */
