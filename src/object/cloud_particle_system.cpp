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

#include "object/cloud_particle_system.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "math/random.hpp"
#include "object/camera.hpp"
#include "supertux/sector.hpp"
#include "supertux/globals.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "video/surface_batch.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"
#include "video/layer.hpp"

CloudParticleSystem::CloudParticleSystem() :
  ParticleSystem(128),
  cloud_image(Surface::from_file("images/particles/cloud.png")),
  m_current_speed_x(1.f),
  m_target_speed_x(1.f),
  m_speed_fade_time_remaining_x(0.f),
  m_current_speed_y(0.f),
  m_target_speed_y(0.f),
  m_speed_fade_time_remaining_y(0.f),
  m_current_amount(15),
  m_current_real_amount(0),
  m_fog_opacity(0.f)
{
  init();
}

CloudParticleSystem::CloudParticleSystem(const ReaderMapping& reader) :
  ParticleSystem(reader, 128),
  cloud_image(Surface::from_file("images/particles/cloud.png")),
  m_current_speed_x(1.f),
  m_target_speed_x(1.f),
  m_speed_fade_time_remaining_x(0.f),
  m_current_speed_y(0.f),
  m_target_speed_y(0.f),
  m_speed_fade_time_remaining_y(0.f),
  m_current_amount(15),
  m_current_real_amount(0),
  m_fog_opacity(0.f)
{
  reader.get("intensity", m_current_amount);
  reader.get("fog_opacity", m_fog_opacity);
  reader.get("x_speed", m_current_speed_x);
  reader.get("y_speed", m_current_speed_y);
  init();
}

CloudParticleSystem::~CloudParticleSystem() {}

void
CloudParticleSystem::init()
{
  virtual_width = 2000.f;

  // Create some random clouds.
  add_clouds(m_current_amount, 0.f);
}

void
CloudParticleSystem::set_fog_opacity(float opacity)
{
  m_fog_opacity = std::clamp(opacity, 0.f, 100.f);
}

ObjectSettings
CloudParticleSystem::get_settings()
{
  ObjectSettings result = ParticleSystem::get_settings();

  result.add_int(_("Amount"), &m_current_amount, "intensity", 15);
  result.add_float(_("Fog Opacity"), &m_fog_opacity, "fog_opacity", 0.f);
  result.add_float(_("X Speed"), &m_current_speed_x, "x_speed", 1.f);
  result.add_float(_("Y Speed"), &m_current_speed_y, "y_speed", 0.f);

  result.reorder({ "intensity", "fog_opacity", "x_speed", "y_speed", "enabled", "name" });

  return result;
}

void
CloudParticleSystem::update(float dt_sec)
{
  if (!enabled)
    return;

  // Update X speed.
  if (m_speed_fade_time_remaining_x > 0.f)
  {
    if (dt_sec >= m_speed_fade_time_remaining_x)
    {
      m_current_speed_x = m_target_speed_x;
      m_speed_fade_time_remaining_x = 0.f;
    }
    else
    {
      float amount = dt_sec / m_speed_fade_time_remaining_x;
      m_current_speed_x += (m_target_speed_x - m_current_speed_x) * amount;
      m_speed_fade_time_remaining_x -= dt_sec;
    }
  }

  // Update Y speed.
  if (m_speed_fade_time_remaining_y > 0.f)
  {
    if (dt_sec >= m_speed_fade_time_remaining_y)
    {
      m_current_speed_y = m_target_speed_y;
      m_speed_fade_time_remaining_y = 0.f;
    }
    else
    {
      float amount = dt_sec / m_speed_fade_time_remaining_y;
      m_current_speed_y += (m_target_speed_y - m_current_speed_y) * amount;
      m_speed_fade_time_remaining_y -= dt_sec;
    }
  }

  auto& cam = Sector::get().get_singleton_by_type<Camera>();
  auto scale = cam.get_current_scale();
  auto screen_width = static_cast<float>(SCREEN_WIDTH) / scale;
  auto screen_height = static_cast<float>(SCREEN_HEIGHT) / scale;

  for (auto& particle : particles)
  {
    auto cloud_particle = dynamic_cast<CloudParticle*>(particle.get());

    if (!cloud_particle)
      continue;

    cloud_particle->pos.x += cloud_particle->speed * dt_sec * m_current_speed_x;
    cloud_particle->pos.y += cloud_particle->speed * dt_sec * m_current_speed_y;

    float texture_height = static_cast<float>(cloud_particle->texture->get_height());
    float texture_width = static_cast<float>(cloud_particle->texture->get_width());

    while (cloud_particle->pos.x < cam.get_translation().x - texture_width)
      cloud_particle->pos.x += screen_width + texture_width * 2.f;

    while (cloud_particle->pos.x > cam.get_translation().x + screen_width)
      cloud_particle->pos.x -= screen_width + texture_width * 2.f;

    while (cloud_particle->pos.y < cam.get_translation().y - texture_height)
      cloud_particle->pos.y += screen_height + texture_height * 2.f;

    while (cloud_particle->pos.y > cam.get_translation().y + screen_height)
      cloud_particle->pos.y -= screen_height + texture_height * 2.f;

    // Update alpha.
    if (cloud_particle->target_time_remaining > 0.f)
    {
      if (dt_sec >= cloud_particle->target_time_remaining)
      {
        cloud_particle->alpha = cloud_particle->target_alpha;
        cloud_particle->target_time_remaining = 0.f;
      }
      else
      {
        float amount = dt_sec / cloud_particle->target_time_remaining;
        cloud_particle->alpha += (cloud_particle->target_alpha - cloud_particle->alpha) * amount;
        cloud_particle->target_time_remaining -= dt_sec;
      }
    }
  }

  // Clear dead clouds.
  // Iterate through the vector backwards to avoid affecting the index of elements
  // after removal, preventing buggy behavior.
  for (int i = static_cast<int>(particles.size()) - 1; i >= 0; --i)
  {
    auto particle = dynamic_cast<CloudParticle*>(particles.at(i).get());

    if (particle->target_alpha == 0.f && particle->target_time_remaining == 0.f)
      particles.erase(particles.begin() + i);
  }
}

void
CloudParticleSystem::apply_fog_effect(DrawingContext& context)
{
  float opacity = m_fog_opacity / 100.0f;

  context.push_transform();
  context.set_translation(Vector(0, 0));
  context.color().draw_filled_rect(context.get_rect(),
    Color(0.3f, 0.38f, 0.4f, opacity),
    LAYER_FOG);
  context.pop_transform();
}

int
CloudParticleSystem::add_clouds(int amount, float fade_time)
{
  int target_amount = std::clamp(m_current_real_amount + amount, min_amount, max_amount);
  int amount_to_add = target_amount - m_current_real_amount;

  for (int i = 0; i < amount_to_add; ++i)
  {
    auto particle = std::make_unique<CloudParticle>();
    // Don't consider the camera, because the Sector might not exist yet
    // Instead, rely on update() to correct this when it will be called.
    particle->pos.x = graphicsRandom.randf(virtual_width);
    particle->pos.y = graphicsRandom.randf(virtual_height);
    particle->texture = cloud_image;
    particle->speed = -graphicsRandom.randf(25.0, 54.0);
    particle->alpha = (fade_time == 0.f) ? 1.f : 0.f;
    particle->target_alpha = 1.f;
    particle->target_time_remaining = fade_time;

    particles.push_back(std::move(particle));
  }

  m_current_real_amount = target_amount;
  return m_current_real_amount;
}

int
CloudParticleSystem::remove_clouds(int amount, float fade_time)
{
  int target_amount = std::clamp(m_current_real_amount - amount, min_amount, max_amount);
  int amount_to_remove = m_current_real_amount - target_amount;

  int i = 0;
  for (; i < amount_to_remove && i < static_cast<int>(particles.size()); ++i)
  {

    auto particle = dynamic_cast<CloudParticle*>(particles.at(i).get());
    if (particle->target_alpha != 1.f || particle->target_time_remaining != 0.f) // Invalid particle.
    {
      --i;
    }
    else
    {
      particle->target_alpha = 0.f;
      particle->target_time_remaining = fade_time;
    }
  }

  return i;
}

void
CloudParticleSystem::fade_speed(float new_speed_x, float new_speed_y, float fade_time)
{
  // No check for enabled; change the fading even if it's disabled.
  // If fade_time is 0 or smaller, update() will never change m_current_speed.

  if (fade_time <= 0.f)
  {
    m_current_speed_x = new_speed_x;
    m_current_speed_y = new_speed_y;
  }

  m_target_speed_x = new_speed_x;
  m_target_speed_y = new_speed_y;
  m_speed_fade_time_remaining_x = m_speed_fade_time_remaining_y = fade_time;
}

void
CloudParticleSystem::fade_amount(int new_amount, float fade_time, float time_between)
{
  // No check for enabled; change the fading even if it's disabled.

  int delta = new_amount - m_current_real_amount;

  if (delta < 0)
  {
    remove_clouds(-delta, fade_time);
  }
  else if (delta > 0)
  {
    add_clouds(delta, fade_time);
  }
}

void
CloudParticleSystem::draw(DrawingContext& context)
{
  if (!enabled)
    return;

  const auto& region = Sector::current()->get_active_region();

  context.push_transform();

  std::unordered_map<SurfacePtr, SurfaceBatch> batches;
  for (const auto& particle : particles)
  {

    if (!region.contains(particle->pos))
      continue;

    if (particle->alpha != 1.f)
    {
      const auto& batch_it = batches.emplace(
        particle->texture->clone(),
        SurfaceBatch(
          particle->texture,
          Color(1.f, 1.f, 1.f, particle->alpha)
        ));
      batch_it.first->second.draw(particle->pos, particle->angle);
    }
    else
    {
      auto it = batches.find(particle->texture);
      if (it == batches.end()) {
        const auto& batch_it = batches.emplace(particle->texture,
          SurfaceBatch(particle->texture));
        batch_it.first->second.draw(particle->pos, particle->angle);
      }
      else
      {
        it->second.draw(particle->pos, particle->angle);
      }
    }
  }

  for (auto& it : batches)
  {
    auto& surface = it.first;
    auto& batch = it.second;
    context.color().draw_surface_batch(surface, batch.move_srcrects(),
      batch.move_dstrects(), batch.move_angles(), batch.get_color(), z_pos);
  }

  apply_fog_effect(context);
  context.pop_transform();
}

void
CloudParticleSystem::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<CloudParticleSystem>("CloudParticleSystem", vm.findClass("ParticleSystem"));

  cls.addFunc("fade_speed", &CloudParticleSystem::fade_speed);
  cls.addFunc("fade_amount", &CloudParticleSystem::fade_amount);
  cls.addFunc("set_amount", &CloudParticleSystem::set_amount);
  cls.addFunc("set_x_speed", &CloudParticleSystem::set_x_speed);
  cls.addFunc("get_x_speed", &CloudParticleSystem::get_x_speed);
  cls.addFunc("set_y_speed", &CloudParticleSystem::set_y_speed);
  cls.addFunc("get_y_speed", &CloudParticleSystem::get_y_speed);
  cls.addFunc("set_fog_opacity", &CloudParticleSystem::set_fog_opacity);
  cls.addFunc("get_fog_opacity", &CloudParticleSystem::get_fog_opacity);
}

/* EOF */
