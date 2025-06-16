//  SuperTux BadGuy GoldBomb - a bomb that throws up coins when exploding
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2013 LMH <lmh.0013@gmail.com>
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


#ifndef HEADER_SUPERTUX_BADGUY_GOLDBOMB_HPP
#define HEADER_SUPERTUX_BADGUY_GOLDBOMB_HPP

#include "badguy/mrbomb.hpp"

class SoundSource;

class GoldBomb final : public MrBomb
{
public:
  GoldBomb(const ReaderMapping& reader);

  virtual void collision_solid(const CollisionHit& hit) override;

  virtual void active_update(float dt_sec) override;
  static std::string class_name() { return "goldbomb"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Gold Bomb"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WalkingBadguy::get_class_types().add(typeid(GoldBomb)); }
  virtual bool is_snipable() const override { return true; }

  int get_coins_worth() const override { return 10; }

  virtual void explode() override;

  GameObjectTypes get_types() const override { return {}; }
  std::string get_default_sprite_name() const override { return m_default_sprite_name; }

private:
  void flee(Direction dir);
  void cornered();
  void recover();
  void normalize();

private:
  enum State : uint8_t {
    GB_STATE_REALIZING = MB_STATE_COUNT,
    GB_STATE_FLEEING,
    GB_STATE_CORNERED,
    GB_STATE_RECOVER
  };

  Timer m_realize_timer;

private:
  GoldBomb(const GoldBomb&) = delete;
  GoldBomb& operator=(const GoldBomb&) = delete;
};

#endif

/* EOF */
