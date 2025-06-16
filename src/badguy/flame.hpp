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

#ifndef HEADER_SUPERTUX_BADGUY_FLAME_HPP
#define HEADER_SUPERTUX_BADGUY_FLAME_HPP

#include "badguy/badguy.hpp"

class SoundSource;

class Flame final : public BadGuy
{
public:
  Flame(const ReaderMapping& reader, int type = -1);

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void active_update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual void kill_fall() override;

  virtual void freeze() override;
  virtual void ignite() override;
  virtual bool is_freezable() const override;
  virtual bool is_flammable() const override;

  virtual ObjectSettings get_settings() override;
  GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

  static std::string class_name() { return "flame"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Flame"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return BadGuy::get_class_types().add(typeid(Flame)); }

  virtual void stop_looping_sounds() override;
  virtual void play_looping_sounds() override;

  virtual void on_flip(float height) override;

public:
  enum Type {
    FIRE,
    GHOST,
    ICE
  };

protected:
  virtual std::vector<Direction> get_allowed_directions() const override;

private:
  float angle;
  float radius;
  float speed;

  std::unique_ptr<SoundSource> sound_source;
  SurfacePtr m_radius_indicator;

private:
  Flame(const Flame&) = delete;
  Flame& operator=(const Flame&) = delete;
};

#endif

/* EOF */
