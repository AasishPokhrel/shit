//  SuperTux - "Will-O-Wisp" Badguy
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

#ifndef HEADER_SUPERTUX_BADGUY_TREEWILLOWISP_HPP
#define HEADER_SUPERTUX_BADGUY_TREEWILLOWISP_HPP

#include "badguy/badguy.hpp"

class GhostTree;
class SoundSource;

class TreeWillOWisp final : public BadGuy
{
public:
  TreeWillOWisp(GhostTree* tree, const Vector& pos, float radius, float speed);
  ~TreeWillOWisp() override;
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(TreeWillOWisp)); }

  virtual void activate() override;
  virtual void active_update(float dt_sec) override;

  virtual bool is_flammable() const override { return false; }
  virtual bool is_freezable() const override { return false; }
  virtual void kill_fall() override { vanish(); }

  virtual void draw(DrawingContext& context) override;

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

  /** make TreeWillOWisp vanish */
  void vanish();
  void start_sucking(const Vector& suck_target);

  void set_color(const Color& color);
  inline Color get_color() const { return color; }

protected:
  virtual bool collides(MovingObject& other, const CollisionHit& hit) const override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;

private:
  enum MyState {
    STATE_DEFAULT, STATE_VANISHING, STATE_SUCKED
  };

public:
  bool was_sucked;

private:
  MyState mystate;

  Color color;
  float angle;
  float radius;
  float speed;

  std::unique_ptr<SoundSource> sound_source;
  GhostTree* tree;

  Vector suck_target;

private:
  TreeWillOWisp(const TreeWillOWisp&) = delete;
  TreeWillOWisp& operator=(const TreeWillOWisp&) = delete;
};

#endif

/* EOF */
