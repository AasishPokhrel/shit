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

#include "object/rain_particle_system.hpp"

#include <assert.h>
#include <math.h>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "math/easing.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "object/camera.hpp"
#include "object/rainsplash.hpp"
#include "supertux/sector.hpp"
#include "util/reader.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

RainParticleSystem::RainParticleSystem() :
  m_current_speed(1.f),
  m_target_speed(1.f),
  m_speed_fade_time_remaining(0.f),
  m_begin_angle(45.f),
  m_current_angle(45.f),
  m_target_angle(45.f),
  m_angle_fade_time_remaining(0.f),
  m_angle_fade_time_total(0.f),
  m_angle_easing(getEasingByName(EaseNone)),
  m_current_amount(1.f),
  m_target_amount(1.f),
  m_amount_fade_time_remaining(0.f),
  m_current_real_amount(0.f)
{
  init();
}

RainParticleSystem::RainParticleSystem(const ReaderMapping& reader) :
  ParticleSystem_Interactive(reader),
  m_current_speed(1.f),
  m_target_speed(1.f),
  m_speed_fade_time_remaining(0.f),
  m_begin_angle(45.f),
  m_current_angle(45.f),
  m_target_angle(45.f),
  m_angle_fade_time_remaining(0.f),
  m_angle_fade_time_total(0.f),
  m_angle_easing(getEasingByName(EaseNone)),
  m_current_amount(1.f),
  m_target_amount(1.f),
  m_amount_fade_time_remaining(0.f),
  m_current_real_amount(0.f)
{
  reader.get("intensity", m_current_amount, 1.f);
  reader.get("angle", m_current_angle, 1.f);
  reader.get("speed", m_current_speed, 1.f);
  init();
}

RainParticleSystem::~RainParticleSystem()
{
}

void RainParticleSystem::init()
{
  rainimages[0] = Surface::from_file("images/particles/rain0.png");
  rainimages[1] = Surface::from_file("images/particles/rain1.png");

  virtual_width = static_cast<float>(SCREEN_WIDTH) * 2.0f;

  // create some random raindrops
  set_amount(m_current_amount);
}

ObjectSettings
RainParticleSystem::get_settings()
{
  ObjectSettings result = ParticleSystem::get_settings();

  result.add_float(_("Intensity"), &m_current_amount, "intensity", 1.f);
  result.add_float(_("Angle"), &m_current_angle, "angle", 1.f);
  result.add_float(_("Speed"), &m_current_speed, "speed", 1.f);

  result.reorder({"intensity", "angle", "speed", "enabled", "name"});

  return result;
}

void RainParticleSystem::set_amount(float amount)
{
  // Don't spawn too many particles to avoid destroying the player's computer
  float real_amount = math::clamp(amount, min_amount, max_amount);

  int old_raindropcount = static_cast<int>(virtual_width*m_current_real_amount/6.0f);
  int new_raindropcount = static_cast<int>(virtual_width*real_amount/6.0f);
  int delta = new_raindropcount - old_raindropcount;

  if (delta > 0) {
    for (int i=0; i<delta; ++i) {
      auto particle = std::make_unique<RainParticle>();
      particle->pos.x = static_cast<float>(graphicsRandom.rand(int(virtual_width)));
      particle->pos.y = static_cast<float>(graphicsRandom.rand(int(virtual_height)));
      int rainsize = graphicsRandom.rand(2);
      particle->texture = rainimages[rainsize];
      do {
        particle->speed = ((static_cast<float>(rainsize) + 1.0f) * 45.0f + graphicsRandom.randf(3.6f));
      } while(particle->speed < 1);
      particles.push_back(std::move(particle));
    }
  } else if (delta < 0) {
    for (int i=0; i>delta; --i) {
      particles.pop_back();
    }
  }

  m_current_real_amount = real_amount;
}

void RainParticleSystem::set_angle(float angle)
{
  for (const auto& particle : particles)
    particle->angle = angle;
}

void RainParticleSystem::update(float dt_sec)
{
  if (!enabled)
    return;

  // Update amount
  if (m_amount_fade_time_remaining > 0.f) {
    if (dt_sec >= m_amount_fade_time_remaining) {
      m_current_amount = m_target_amount;
      m_amount_fade_time_remaining = 0.f;
      // Test below
      /*if (m_current_amount > 1.1f) {
        m_target_amount = 0.1f;
      } else {
        m_target_amount = 5.f;
      }
      m_amount_fade_time_remaining = 2.f;*/
      // Test above
    } else {
      float amount = dt_sec / m_amount_fade_time_remaining;
      m_current_amount += (m_target_amount - m_current_amount) * amount;
      m_amount_fade_time_remaining -= dt_sec;
    }
  }

  set_amount(m_current_amount);

  // Update speed
  if (m_speed_fade_time_remaining > 0.f) {
    if (dt_sec >= m_speed_fade_time_remaining) {
      m_current_speed = m_target_speed;
      m_speed_fade_time_remaining = 0.f;
    } else {
      float amount = dt_sec / m_speed_fade_time_remaining;
      m_current_speed += (m_target_speed - m_current_speed) * amount;
      m_speed_fade_time_remaining -= dt_sec;
    }
  }

  // Update angle
  if (m_angle_fade_time_remaining > 0.f) {
    if (dt_sec >= m_angle_fade_time_remaining) {
      m_current_angle = m_target_angle;
      m_angle_fade_time_remaining = 0.f;
    } else {
      m_angle_fade_time_remaining -= dt_sec;
      float progress = 1.f - m_angle_fade_time_remaining / m_angle_fade_time_total;
      progress = static_cast<float>(m_angle_easing(static_cast<double>(progress)));
      m_current_angle = progress * (m_target_angle - m_begin_angle) + m_begin_angle;
    }
    set_angle(m_current_angle);
  }

  const auto& cam_translation = Sector::get().get_camera().get_translation();
  float movement_multiplier = dt_sec * Sector::get().get_gravity() * m_current_speed * 1.41421353f;
  float abs_x = cam_translation.x;
  float abs_y = cam_translation.y;

  for (auto& it : particles) {
    auto particle = static_cast<RainParticle*>(it.get());

    float movement = particle->speed * movement_multiplier;
    particle->pos.y += movement * cosf((particle->angle + 45.f) * 3.14159265f / 180.f);
    particle->pos.x -= movement * sinf((particle->angle + 45.f) * 3.14159265f / 180.f);
    int col = collision(particle, Vector(-movement, movement));
    if ((particle->pos.y > static_cast<float>(SCREEN_HEIGHT) + abs_y) || (col >= 0)) {
      //Create rainsplash
      if ((particle->pos.y <= static_cast<float>(SCREEN_HEIGHT) + abs_y) && (col >= 1)){
        bool vertical = (col == 2);
        if (!vertical) { //check if collision happened from above
          int splash_x, splash_y; // move outside if statement when
                                  // uncommenting the else statement below.
          splash_x = int(particle->pos.x);
          splash_y = int(particle->pos.y) - (int(particle->pos.y) % 32) + 32;
          Sector::get().add<RainSplash>(Vector(static_cast<float>(splash_x), static_cast<float>(splash_y)),
                                             vertical);
        }
        // Uncomment the following to display vertical splashes, too
        /* else {
           splash_x = int(particle->pos.x) - (int(particle->pos.x) % 32) + 32;
           splash_y = int(particle->pos.y);
           Sector::get().add<RainSplash>(Vector(splash_x, splash_y),vertical);
           } */
      }
      int new_x = graphicsRandom.rand(int(virtual_width)) + int(abs_x);
      int new_y = 0;
      //FIXME: Don't move particles over solid tiles
      particle->pos.x = static_cast<float>(new_x);
      particle->pos.y = static_cast<float>(new_y);
    }
  }
}

void RainParticleSystem::fade_speed(float new_speed, float fade_time)
{
  // No check to enabled; change the fading even if it's disabled

  // If time is 0 (or smaller?), then update() will never change m_current_speed
  if (fade_time <= 0.f)
  {
    m_current_speed = new_speed;
  }

  m_target_speed = new_speed;
  m_speed_fade_time_remaining = fade_time;
}

void RainParticleSystem::fade_amount(float new_amount, float fade_time)
{
  // No check to enabled; change the fading even if it's disabled

  // If time is 0 (or smaller?), then update() will never change m_current_amount
  if (fade_time <= 0.f)
  {
    m_current_amount = new_amount;
  }

  m_target_amount = new_amount;
  m_amount_fade_time_remaining = fade_time;
}

void
RainParticleSystem::fade_angle(float angle, float time, const std::string& ease)
{
  fade_angle(angle, time, getEasingByName(EasingMode_from_string(ease)));
}

void RainParticleSystem::fade_angle(float new_angle, float fade_time, easing ease_func)
{
  // No check to enabled; change the fading even if it's disabled

  // If time is 0 (or smaller?), then update() will never change m_current_amount
  if (fade_time <= 0.f)
  {
    m_current_angle = new_angle - 45.f;
  }

  m_begin_angle = m_current_angle;
  m_target_angle = new_angle - 45.f;
  m_angle_fade_time_total = fade_time;
  m_angle_fade_time_remaining = fade_time;
  m_angle_easing = ease_func;
}

void RainParticleSystem::draw(DrawingContext& context)
{
  ParticleSystem_Interactive::draw(context);

  if (!enabled)
    return;

  float opacity = fog_max_value * (m_current_amount - fog_start_amount) / (max_amount - fog_start_amount);
  if (opacity < 0.f)
    opacity = 0.f;
  if (opacity > 1.f)
    opacity = 1.f;

  context.push_transform();
  context.set_translation(Vector(0, 0));
  context.color().draw_filled_rect(context.get_rect(),
                                   Color(0.3f, 0.38f, 0.4f, opacity),
                                   z_pos);
  context.pop_transform();
}


void
RainParticleSystem::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<RainParticleSystem>("RainParticleSystem", vm.findClass("ParticleSystem"));

  cls.addFunc("fade_speed", &RainParticleSystem::fade_speed);
  cls.addFunc("fade_amount", &RainParticleSystem::fade_amount);
  cls.addFunc<void, RainParticleSystem, float, float, const std::string&>("fade_angle", &RainParticleSystem::fade_angle);
}

/* EOF */
