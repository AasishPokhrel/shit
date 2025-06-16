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

#ifndef HEADER_SUPERTUX_BADGUY_STALACTITE_HPP
#define HEADER_SUPERTUX_BADGUY_STALACTITE_HPP

#include "object/sticky_object.hpp"

class Stalactite : public StickyBadguy
{
public:
  Stalactite(const ReaderMapping& mapping);

  virtual void active_update(float dt_sec) override;
  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& other, const CollisionHit& hit) override;
  virtual HitResponse collision_bullet(Bullet& bullet, const CollisionHit& hit) override;

  virtual GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

  virtual void kill_fall() override;
  virtual void draw(DrawingContext& context) override;
  virtual void deactivate() override;

  static std::string class_name() { return "stalactite"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Stalactite"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return StickyBadguy::get_class_types().add(typeid(Stalactite)); }

  virtual ObjectSettings get_settings() override;

  virtual void on_flip(float height) override;

  void squish();

protected:
  std::vector<Direction> get_allowed_directions() const override;

protected:
  enum StalactiteType {
    ICE,
    ROCK
  };
  enum StalactiteState {
    STALACTITE_HANGING,
    STALACTITE_SHAKING,
    STALACTITE_FALLING,
    STALACTITE_SQUISHED
  };

protected:
  Timer timer;
  StalactiteState state;
  Vector shake_delta;

private:
  Stalactite(const Stalactite&) = delete;
  Stalactite& operator=(const Stalactite&) = delete;
};

#endif

/* EOF */
