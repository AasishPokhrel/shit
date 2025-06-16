//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_OBJECT_RAIN_PARTICLE_SYSTEM_HPP
#define HEADER_SUPERTUX_OBJECT_RAIN_PARTICLE_SYSTEM_HPP

#include "object/particlesystem_interactive.hpp"

#include "math/easing.hpp"
#include "video/surface_ptr.hpp"

/**
 * @scripting
 * @summary A ""RainParticleSystem"" that was given a name can be controlled by scripts.
 * @instances A ""RainParticleSystem"" is instantiated by placing a definition inside a level.
              It can then be accessed by its name from a script or via ""sector.name"" from the console.
 */
class RainParticleSystem final : public ParticleSystem_Interactive
{
public:
  static void register_class(ssq::VM& vm);

public:
  RainParticleSystem();
  RainParticleSystem(const ReaderMapping& reader);
  ~RainParticleSystem() override;

  virtual void draw(DrawingContext& context) override;

  void init();
  virtual void update(float dt_sec) override;

  static std::string class_name() { return "particles-rain"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual std::string get_exposed_class_name() const override { return "RainParticleSystem"; }
  static std::string display_name() { return _("Rain Particles"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return ParticleSystem_Interactive::get_class_types().add(typeid(RainParticleSystem)); }
  virtual ObjectSettings get_settings() override;

  /**
   * @scripting
   * @description Smoothly changes the rain speed to the given value in ""time"" seconds.
   * @param float $speed
   * @param float $time
   */
  void fade_speed(float speed, float time);
  /**
   * @scripting
   * @description Smoothly changes the amount of particles to the given value in ""time"" seconds.
   * @param float $amount
   * @param float $time
   */
  void fade_amount(float amount, float time);
  /**
   * @scripting
   * @description Smoothly changes the angle of the rain the given value in ""time"" seconds, according to the provided easing function.
   * @param float $angle
   * @param float $time
   * @param string $ease
   */
  void fade_angle(float angle, float time, const std::string& ease);

  /** Smoothly changes the angle of the rain the given value in ""time"" seconds, according to the provided easing function. */
  void fade_angle(float new_angle, float fade_time, easing ease_func);

  virtual const std::string get_icon_path() const override {
    return "images/engine/editor/rain.png";
  }

  // Minimum and maximum multiplier for the amount of particles (intensity)
  static float constexpr const max_amount = 5.0f;
  static float constexpr const min_amount = 0.1f;

  // Minimum value of m_current_amount for the fog to be > 0
  static float constexpr const fog_start_amount = 1.0f;

  // When m_current_amount == max_amount, fog is this value
  static float constexpr const fog_max_value = 0.6f;

private:
  void set_amount(float amount);
  void set_angle(float angle);

private:
  class RainParticle : public Particle
  {
  public:
    float speed;

    RainParticle() :
      speed()
    {}
  };

  SurfacePtr rainimages[2];

  float m_current_speed;
  float m_target_speed;
  float m_speed_fade_time_remaining;

  float m_begin_angle;
  float m_current_angle;
  float m_target_angle;
  float m_angle_fade_time_remaining;
  float m_angle_fade_time_total;
  easing m_angle_easing;

  float m_current_amount;
  float m_target_amount;
  float m_amount_fade_time_remaining;
  
  float m_current_real_amount;

private:
  RainParticleSystem(const RainParticleSystem&) = delete;
  RainParticleSystem& operator=(const RainParticleSystem&) = delete;
};

#endif

/* EOF */
