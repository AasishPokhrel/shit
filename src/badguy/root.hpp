//  SuperTux - Corrupted Root
//  Copyright (C) 2023 MatusGuy <matusguy@supertuxproject.org>
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

#ifndef HEADER_SUPERTUX_BADGUY_ROOT_HPP
#define HEADER_SUPERTUX_BADGUY_ROOT_HPP

#include "badguy/badguy.hpp"

#include "supertux/timer.hpp"

class Root final : public BadGuy
{
public:
  Root(const ReaderMapping& reader);
  Root(const Vector& pos, Direction dir, const std::string& sprite,
    float delay = -1, bool play_sound = true, bool auto_construct = true, float base_delay = 0.0f);

  void construct();

  virtual void initialize() override;
  virtual void draw(DrawingContext& context) override;
  virtual void active_update(float dt_sec) override;
  virtual HitResponse collision_badguy(BadGuy& other, const CollisionHit& hit) override;
  virtual void kill_fall() override;

  static std::string class_name() { return "root"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Root"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Root)); }

  virtual bool is_flammable() const override { return false; }
  virtual bool is_freezable() const override { return false; }
  virtual bool is_snipable() const override { return false; }

  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  enum State { STATE_PRE_HATCHING, STATE_HATCHING, STATE_APPEARING, STATE_RETREATING, STATE_FADING_OUT };

private:
  SurfacePtr m_base_surface;
  Timer m_timer;
  State m_state;
  float m_delay;
  Timer m_initial_timer;
  float m_base_delay;
  Timer m_fade_timer;
  float m_base_alpha;
  float m_maxheight;
  bool m_play_sound;
  bool m_show_base;

private:
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

#endif

/* EOF */
