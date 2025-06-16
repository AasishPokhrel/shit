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

#include "object/snow_particle_system.hpp"

#include <assert.h>
#include <math.h>

#include "math/random.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"
  
static const float DECAY_RATIO = 0.2f; // Ratio of attack speed to decay speed.
static const float WOBBLE_DECAY = 0.99f; // Wobble decays exponentially by this much each tick.
static const float WOBBLE_FACTOR = 4 * .005f; // Wobble approaches drift_speed by this much each tick.

SnowParticleSystem::SnowParticleSystem() :
  m_state(RELEASING),
  m_timer(),
  m_gust_onset(0),
  m_gust_current_velocity(0),
  m_wind_speed(),
  m_epsilon(),
  m_spin_speed(),
  m_state_length(),
  m_snowimages()
{
  init();
}

SnowParticleSystem::SnowParticleSystem(const ReaderMapping& reader) :
  ParticleSystem(reader),
  m_state(RELEASING),
  m_timer(),
  m_gust_onset(0),
  m_gust_current_velocity(0),
  m_wind_speed(),
  m_epsilon(),
  m_spin_speed(),
  m_state_length(),
  m_snowimages()
{
  reader.get("state_length", m_state_length, 5.0f);
  reader.get("wind_speed", m_wind_speed, 30.0f);
  reader.get("spin_speed", m_spin_speed, 60.0f);
  reader.get("epsilon", m_epsilon, 0.5f);
  init();
}

SnowParticleSystem::~SnowParticleSystem()
{
}

void
SnowParticleSystem::init()
{
  m_snowimages[0] = Surface::from_file("images/particles/snow2.png");
  m_snowimages[1] = Surface::from_file("images/particles/snow1.png");
  m_snowimages[2] = Surface::from_file("images/particles/snow0.png");

  virtual_width = static_cast<float>(SCREEN_WIDTH) * 2.0f;

  m_timer.start(.01f);

  // Create random snowflakes.
  int snowflakecount = static_cast<int>(virtual_width / 10.0f);
  for (int i = 0; i < snowflakecount; ++i)
  {
    auto particle = std::make_unique<SnowParticle>();
    int snowsize = graphicsRandom.rand(3);

    particle->pos.x = graphicsRandom.randf(virtual_width);
    particle->pos.y = graphicsRandom.randf(static_cast<float>(SCREEN_HEIGHT));
    particle->anchorx = particle->pos.x + (graphicsRandom.randf(-0.5, 0.5) * 16);
    // Drift will change with wind gusts.
    particle->drift_speed = graphicsRandom.randf(-0.5f, 0.5f) * 0.3f;
    particle->wobble = 0.0;

    particle->texture = m_snowimages[snowsize];
    particle->flake_size = static_cast<int>(powf(static_cast<float>(snowsize) + 3.0f, 4.0f)); // Since it ranges from 0 to 2.

    particle->speed = 6.32f * (1.0f + (2.0f - static_cast<float>(snowsize)) / 2.0f + graphicsRandom.randf(1.8f));

    // Spinning.
    particle->angle = graphicsRandom.randf(360.0);
    particle->spin_speed = graphicsRandom.randf(-m_spin_speed, m_spin_speed);

    particles.push_back(std::move(particle));
  }
}

ObjectSettings
SnowParticleSystem::get_settings()
{
  ObjectSettings result = ParticleSystem::get_settings();

  result.add_float(_("Epsilon"), &m_epsilon, "epsilon", 0.5f);
  result.add_float(_("Spin Speed"), &m_spin_speed, "spin_speed", 60.0f);
  result.add_float(_("State Length"), &m_state_length, "state_length", 5.0f);
  result.add_float(_("Wind Speed"), &m_wind_speed, "wind_speed", 30.0f);

  result.reorder({ "epsilon", "spin_speed", "state_length", "wind_speed", "enabled", "name" });

  return result;
}

void
SnowParticleSystem::update(float dt_sec)
{
  if (!enabled)
    return;
  
  // Simple ADSR wind gusts.

  if (m_timer.check()) 
  {
    // Change state
    m_state = static_cast<State>((m_state + 1) % MAX_STATE);

    if (m_state == RESTING) 
    {
      // Stop wind.
      m_gust_current_velocity = 0;
      // New wind strength.
      m_gust_onset = -m_wind_speed;
    }
    m_timer.start(graphicsRandom.randf(m_state_length));
  }

  // Update velocities.
  switch (m_state) 
  {
    case ATTACKING:
      m_gust_current_velocity += m_gust_onset * dt_sec;
      break;
    case DECAYING:
      m_gust_current_velocity -= m_gust_onset * dt_sec * DECAY_RATIO;
      break;
    case RELEASING:
      // Uses current time/velocity instead of constants.
      m_gust_current_velocity -= m_gust_current_velocity * dt_sec / m_timer.get_timeleft();
      break;
    case SUSTAINING:
    case RESTING:
      break;
    default:
      assert(false);
  }

  float sq_g = sqrtf(Sector::get().get_gravity());

  for (auto& part : particles)
  {
    auto particle = dynamic_cast<SnowParticle*>(part.get());
    if (!particle)
      continue;

    float anchor_delta;

    // Falling.
    particle->pos.y += particle->speed * dt_sec * sq_g;
    // Drifting (speed approaches wind at a rate dependent on flake size).
    particle->drift_speed += (m_gust_current_velocity - particle->drift_speed) / static_cast<float>(particle->flake_size) + graphicsRandom.randf(-m_epsilon, m_epsilon);
    particle->anchorx += particle->drift_speed * dt_sec;
    // Wobbling (particle approaches anchorx).
    particle->pos.x += particle->wobble * dt_sec * sq_g;
    anchor_delta = (particle->anchorx - particle->pos.x);
    particle->wobble += (WOBBLE_FACTOR * anchor_delta) + graphicsRandom.randf(-m_epsilon, m_epsilon);
    particle->wobble *= WOBBLE_DECAY;
    // Spinning.
    particle->angle += particle->spin_speed * dt_sec;
    particle->angle = fmodf(particle->angle, 360.0);
  }
}

/* EOF */
