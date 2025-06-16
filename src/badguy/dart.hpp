//  Dart - Sharp projectile commonly shot from DartTrap
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

#ifndef HEADER_SUPERTUX_BADGUY_DART_HPP
#define HEADER_SUPERTUX_BADGUY_DART_HPP

#include "badguy/badguy.hpp"

class SoundSource;

/** Badguy "Dart" - Sharp projectile commonly shot from DartTrap */
class Dart final : public BadGuy
{
public:
  Dart(const ReaderMapping& reader);
  Dart(const Vector& pos, Direction d, const BadGuy* parent, const std::string& sprite = "images/creatures/darttrap/granito/root_dart.sprite");

  virtual void initialize() override;
  virtual void activate() override;
  virtual void deactivate() override;

  virtual void active_update(float dt_sec) override;

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;

  virtual bool updatePointers(const GameObject* from_object, GameObject* to_object);
  static std::string class_name() { return "dart"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Dart"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Dart)); }

  virtual bool is_flammable() const override;

  virtual bool is_hurtable() const override { return false; }

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

  virtual void on_flip(float height) override;

  inline void set_flip(Flip flip) { m_flip = flip; }

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

protected:
  const BadGuy* parent; /**< collisions with this BadGuy will be ignored */
  std::unique_ptr<SoundSource> sound_source; /**< SoundSource for ambient sound */

private:
  Dart(const Dart&) = delete;
  Dart& operator=(const Dart&) = delete;
};

#endif

/* EOF */
