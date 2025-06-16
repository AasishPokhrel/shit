//  SuperTux
//  Copyright (C) 2024 Daniel Ward <weluvgoatz@gmail.com>
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

#ifndef HEADER_SUPERTUX_BADGUY_BOSS_HPP
#define HEADER_SUPERTUX_BADGUY_BOSS_HPP

#include "badguy/badguy.hpp"

// This is a class for our (currently two) bosses, but can apply to future ones also.
// Includes information on lives, HUD, pinch mode activation, and death behavior.

class Boss : public BadGuy
{
public:
  Boss(const ReaderMapping& mapping, const std::string& sprite_name, int layer = LAYER_OBJECTS);
  virtual void boss_update(float dt_sec);
  virtual void draw(DrawingContext& context) override;
  void draw_hit_points(DrawingContext& context);
  virtual ObjectSettings get_settings() override;
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Boss)); }

  virtual bool is_flammable() const override { return false; }
  virtual bool is_freezable() const override { return false; }

protected:
  int m_lives;
  int m_max_lives;
  int m_pinch_lives;
  SurfacePtr m_hud_head;
  std::string m_hud_icon;
  bool m_pinch_mode;
  std::string m_pinch_activation_script;

private:
  Boss(const Boss&) = delete;
  Boss& operator=(const Boss&) = delete;
};

#endif

/* EOF */
