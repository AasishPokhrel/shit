//  SuperTux
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

#ifndef HEADER_SUPERTUX_BADGUY_GHOUL_HPP
#define HEADER_SUPERTUX_BADGUY_GHOUL_HPP

#include "badguy/badguy.hpp"
#include "object/path_object.hpp"

// FIXME: Ghoul inherits PathObject, but does not override get_settings() to add
//        the missing options.
class Ghoul final : public BadGuy,
                    public PathObject
{
public:
  Ghoul(const ReaderMapping& reader);
  static std::string class_name() { return "ghoul"; }
  static std::string display_name() { return _("Ghoul"); }
  std::string get_class_name() const override { return class_name(); }
  std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(PathObject)).add(typeid(Ghoul)); }
  bool is_freezable() const override;
  bool is_flammable() const override;
  virtual bool is_snipable() const override { return true; }

  void finish_construction() override;

  void activate() override;
  void deactivate() override;
  void active_update(float dt_sec) override;
  
  void goto_node(int node_idx);
  void set_state(const std::string& state);
  void start_moving();
  void stop_moving();

  void move_to(const Vector& pos) override;

protected:
  bool collision_squished(MovingObject& object) override;
  std::vector<Direction> get_allowed_directions() const override;
  
private:
  enum MyState {
    STATE_STOPPED, STATE_IDLE, STATE_TRACKING, STATE_PATHMOVING, STATE_PATHMOVING_TRACK
  };
  
private:
  MyState m_mystate;
  float m_flyspeed;
  float m_track_range;
  
private:
  Ghoul(const Ghoul&) = delete;
  Ghoul& operator=(const Ghoul&) = delete;
};

#endif

/* EOF */
