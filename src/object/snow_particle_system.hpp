//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2024 bruhmoent
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

#ifndef HEADER_SUPERTUX_OBJECT_SNOW_PARTICLE_SYSTEM_HPP
#define HEADER_SUPERTUX_OBJECT_SNOW_PARTICLE_SYSTEM_HPP

#include "object/particlesystem.hpp"
#include "supertux/timer.hpp"

class ReaderMapping;

class SnowParticleSystem final : public ParticleSystem
{
public:
  SnowParticleSystem();
  SnowParticleSystem(const ReaderMapping& reader);
  ~SnowParticleSystem() override;

  virtual void update(float dt_sec) override;

  static std::string class_name() { return "particles-snow"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Snow Particles"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return ParticleSystem::get_class_types().add(typeid(SnowParticleSystem)); }
  virtual ObjectSettings get_settings() override;

  virtual const std::string get_icon_path() const override
  {
    return "images/engine/editor/snow.png";
  }

private:
  void init();

  class SnowParticle : public Particle
  {
  public:
    float speed;
    float wobble;
    float anchorx;
    float drift_speed;

    // Turning speed.
    float spin_speed;

    // For inertia.
    unsigned int flake_size;

    SnowParticle() :
      speed(),
      wobble(),
      anchorx(),
      drift_speed(),
      spin_speed(),
      flake_size()
    {}
  };

  // Wind is simulated in discrete "gusts",
  // gust states:
  enum State {
    ATTACKING,
    DECAYING,
    SUSTAINING,
    RELEASING,
    RESTING,
    MAX_STATE
  };

private:
  State m_state;

  // Gust state delay timer.
  Timer m_timer;

  // Peak magnitude of gust is gust_onset * randf(5).
  float m_gust_onset;

  // Current blowing velocity of gust.
  float m_gust_current_velocity;

  float m_wind_speed;
  float m_epsilon; // Velocity changes by up to this much each tick.
  float m_spin_speed;
  float m_state_length; // Interval for how long to affect the particles with wind.

  SurfacePtr m_snowimages[3];
  
private:
  SnowParticleSystem(const SnowParticleSystem&) = delete;
  SnowParticleSystem& operator=(const SnowParticleSystem&) = delete;
};

#endif

/* EOF */
