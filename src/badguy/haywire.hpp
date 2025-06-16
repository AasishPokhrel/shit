//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#ifndef HEADER_SUPERTUX_BADGUY_HAYWIRE_HPP
#define HEADER_SUPERTUX_BADGUY_HAYWIRE_HPP

#include "badguy/walking_badguy.hpp"

class SoundSource;

class Haywire final : public WalkingBadguy
{
public:
  Haywire(const ReaderMapping& reader);

  virtual void kill_fall() override;
  virtual void ignite() override;

  virtual void active_update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual bool is_freezable() const override;
  virtual void freeze() override;

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;

  static std::string class_name() { return "haywire"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Haywire"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(Haywire)); }
  virtual bool is_snipable() const override { return true; }

  inline bool is_exploding() const { return m_is_exploding; }

protected:
  virtual bool collision_squished(MovingObject& object) override;
  virtual void collision_solid(const CollisionHit& hit) override;

private:
  Direction get_player_direction(const Player* player) const;

  void start_exploding();
  void stop_exploding();

private:
  bool m_is_exploding;
  float time_until_explosion;
  bool is_stunned;
  float time_stunned;
  SpritePtr m_exploding_sprite;

  bool m_jumping;
  Timer m_skid_timer;
  Direction m_last_player_direction;

  std::unique_ptr<SoundSource> ticking;
  std::unique_ptr<SoundSource> grunting;
  
  Timer stomped_timer;

private:
  Haywire(const Haywire&) = delete;
  Haywire& operator=(const Haywire&) = delete;
};

#endif

/* EOF */
