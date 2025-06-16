//  SuperTux - Boss "GhostTree"
//  Copyright (C) 2007 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_GHOSTTREE_HPP
#define HEADER_SUPERTUX_BADGUY_GHOSTTREE_HPP

#include "badguy/boss.hpp"

class TreeWillOWisp;
class Lantern;

class GhostTree final : public Boss
{
public:
  GhostTree(const ReaderMapping& mapping);

  virtual void kill_fall() override { }

  virtual void activate() override;
  virtual void active_update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual bool collides(MovingObject& other, const CollisionHit& hit) const override;
  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;

  static std::string class_name() { return "ghosttree"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Ghost Tree"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return Boss::get_class_types().add(typeid(GhostTree)); }

  virtual void on_flip(float height) override;

  void willowisp_died(TreeWillOWisp* willowisp);
  void die();

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  enum MyState {
    STATE_IDLE, STATE_SUCKING, STATE_SWALLOWING, STATE_DYING
  };

private:
  bool is_color_deadly(Color color) const;
  void spawn_lantern();

private:
  MyState mystate;
  Timer willowisp_timer;
  float willo_spawn_y;
  float willo_radius;
  float willo_speed;
  int   willo_color;

  SpritePtr glow_sprite;
  Timer colorchange_timer;
  Timer suck_timer;
  Timer root_timer;
  int   treecolor;
  Color suck_lantern_color;

  bool m_taking_life;

  Lantern* suck_lantern; /**< Lantern that is currently being sucked in */

  std::vector<TreeWillOWisp*> willowisps;

private:
  GhostTree(const GhostTree&) = delete;
  GhostTree& operator=(const GhostTree&) = delete;
};

#endif

/* EOF */
