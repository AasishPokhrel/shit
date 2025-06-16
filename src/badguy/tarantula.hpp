//  SuperTux
//  Copyright (C) 2024 MatusGuy <matusguy@supertuxproject.org>
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

#ifndef HEADER_SUPERTUX_BADGUY_TARANTULA_HPP
#define HEADER_SUPERTUX_BADGUY_TARANTULA_HPP

#include "badguy/badguy.hpp"

class Tarantula final : public BadGuy
{
public:
  Tarantula(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual void draw(DrawingContext& context) override;

  virtual void freeze() override;
  virtual void unfreeze(bool) override;
  virtual bool is_freezable() const override;

  virtual std::string get_overlay_size() const override { return "3x3"; }
  static std::string class_name() { return "tarantula"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Tarantula"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Tarantula)); }
  virtual bool is_snipable() const override;

  virtual GameObjectTypes get_types() const override;
  virtual std::string get_default_sprite_name() const override;

  virtual ObjectSettings get_settings() override;

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;
  virtual bool collision_squished(MovingObject& object) override;

protected:
  enum Type { TARANTULA, SPIDERMITE };
  enum State
  {
    STATE_IDLE,
    STATE_APPROACHING,
    STATE_DROPPING,
    STATE_HANG_UP,
    STATE_HANG_DOWN,
    STATE_RETREATING
  };
  enum ApproachResponse
  {
    NONE,
    APPROACH,
    DROP
  };

private:
  ApproachResponse try_approach();
  bool try_drop();

  bool hang_to(float height, float time, bool calctime, State nextstate,
               EasingMode easing, const std::string& action);
  float calculate_time(float div);

  State m_state;
  Timer m_timer;
  SurfacePtr m_silk;
  float m_target_height;
  float m_last_height;
  bool m_was_grabbed;
  bool m_retreat;
  bool m_attach_ceiling;
  bool m_static;
  float m_ground_height;

private:
  Tarantula(const Tarantula&) = delete;
  Tarantula& operator=(const Tarantula&) = delete;
};

#endif

/* EOF */
