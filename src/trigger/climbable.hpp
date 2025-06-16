//  SuperTux - Climbable area
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

#ifndef HEADER_SUPERTUX_TRIGGER_CLIMBABLE_HPP
#define HEADER_SUPERTUX_TRIGGER_CLIMBABLE_HPP

#include "trigger/trigger_base.hpp"

#include <vector>

#include "supertux/timer.hpp"

class Color;

class Climbable final : public Trigger
{
private:
  struct ClimbPlayer
  {
    Player* m_player;
    std::unique_ptr<Timer> m_activate_try_timer;
  };

private:
  static Color text_color;

public:
  Climbable(const ReaderMapping& reader);
  ~Climbable() override;

  static std::string class_name() { return "climbable"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Climbable"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual bool has_variable_size() const override { return true; }
  virtual GameObjectClasses get_class_types() const override { return Trigger::get_class_types().add(typeid(Climbable)); }

  virtual ObjectSettings get_settings() override;

  virtual void event(Player& player, EventType type) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  /** returns true if the player is within bounds of the Climbable */
  bool may_climb(const Player& player) const;

protected:
  std::vector<Player*> climbed_by; /** contains players who's currently climbing us, empty if nobody is. */
  std::vector<ClimbPlayer> trying_to_climb; /** Contains players that are trying to climb */
  std::string message;

private:
  Climbable(const Climbable&) = delete;
  Climbable& operator=(const Climbable&) = delete;
};

#endif

/* EOF */
