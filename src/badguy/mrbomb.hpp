//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_BADGUY_MRBOMB_HPP
#define HEADER_SUPERTUX_BADGUY_MRBOMB_HPP

#include "badguy/walking_badguy.hpp"

class SoundSource;

class MrBomb : public WalkingBadguy
{
public:
  MrBomb(const ReaderMapping& reader);
  MrBomb(const ReaderMapping& reader, const std::string& sprite,
         const std::string& glow_sprite = "images/creatures/mr_bomb/ticking_glow/ticking_glow.sprite");

  virtual void collision_solid(const CollisionHit& hit) override;
  virtual HitResponse collision(MovingObject& object, const CollisionHit& hit) override;
  virtual HitResponse collision_player(Player& player, const CollisionHit& hit) override;
  virtual HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit) override;

  virtual void active_update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual void grab(MovingObject& object, const Vector& pos, Direction dir) override;
  virtual void ungrab(MovingObject& object, Direction dir) override;
  virtual bool is_portable() const override;

  virtual void freeze() override;
  virtual bool is_freezable() const override;

  inline bool is_ticking() const { return m_state == MB_STATE_TICKING; }
  virtual void trigger(Player* player);
  virtual void explode();

  virtual void kill_fall() override;
  virtual void ignite() override;
  static std::string class_name() { return "mrbomb"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Mr. Bomb"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(MrBomb)); }
  virtual bool is_snipable() const override { return true; }

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

  GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

protected:
  void update_ticking(float dt_sec);

  virtual bool collision_squished(MovingObject& object) override;

protected:
  enum State : uint8_t {
    MB_STATE_NORMAL,
    MB_STATE_TICKING,

    MB_STATE_COUNT
  };

  uint8_t m_state;

  std::unique_ptr<SoundSource> m_ticking_sound;
  SpritePtr m_exploding_sprite;

private:
  enum Type {
    NORMAL,
    CLASSIC
  };

private:
  MrBomb(const MrBomb&) = delete;
  MrBomb& operator=(const MrBomb&) = delete;

};

#endif

/* EOF */
