//  SuperTux
//  Copyright (C) 2023 MatusGuy
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

#ifndef HEADER_SUPERTUX_BADGUY_CORRUPTED_GRANITO_BIG_HPP
#define HEADER_SUPERTUX_BADGUY_CORRUPTED_GRANITO_BIG_HPP

#include "badguy/badguy.hpp"

/** Granito, which explodes into shards when player comes close to it. */
class CorruptedGranitoBig final : public BadGuy
{
public:
  explicit CorruptedGranitoBig(const ReaderMapping& reader);

  virtual void initialize() override;
  virtual void active_update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual void kill_fall() override;

  static std::string class_name() { return "corrupted_granito_big"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Corrupted Big Granito"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(CorruptedGranitoBig)); }

  virtual bool is_snipable()  const override { return false; }
  virtual bool is_freezable() const override { return false; }
  virtual bool is_flammable() const override { return false; }

private:
  bool try_cracking();
  void crack();
  void crack_effects(int particles);

private:
  enum State
  {
    STATE_READY,
    STATE_CRACK1,
    STATE_CRACK2,
    STATE_BROKEN
  };

  State m_state;
  Timer m_crack_timer;

  Timer m_shake_timer;
  float m_shake_delta;

  SpritePtr m_rock_particles;

private:
  CorruptedGranitoBig(const CorruptedGranitoBig&) = delete;
  CorruptedGranitoBig& operator=(const CorruptedGranitoBig&) = delete;
};

#endif

/* EOF */
