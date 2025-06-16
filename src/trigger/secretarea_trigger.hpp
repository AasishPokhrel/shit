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

#ifndef HEADER_SUPERTUX_TRIGGER_SECRETAREA_TRIGGER_HPP
#define HEADER_SUPERTUX_TRIGGER_SECRETAREA_TRIGGER_HPP

#include "trigger/trigger_base.hpp"

#include "supertux/timer.hpp"

class Color;

class SecretAreaTrigger final : public Trigger
{
public:
  static Color text_color;

public:
  SecretAreaTrigger(const ReaderMapping& reader);

  static std::string class_name() { return "secretarea"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Secret Area"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual bool has_variable_size() const override { return true; }
  virtual GameObjectClasses get_class_types() const override { return Trigger::get_class_types().add(typeid(SecretAreaTrigger)); }

  virtual ObjectSettings get_settings() override;

  virtual void event(Player& player, EventType type) override;
  virtual void draw(DrawingContext& context) override;

  inline const std::string& get_fade_tilemap_name() const { return fade_tilemap; }

private:
  Timer message_timer;
  bool message_displayed;
  std::string message; /**< message to display, default "You found a secret area!" */
  std::string fade_tilemap; /**< tilemap to fade away when trigger is activated, or empty if you don't care */
  std::string script; /**< optional script to run when trigger is activated */

private:
  SecretAreaTrigger(const SecretAreaTrigger&) = delete;
  SecretAreaTrigger& operator=(const SecretAreaTrigger&) = delete;
};

#endif

/* EOF */
