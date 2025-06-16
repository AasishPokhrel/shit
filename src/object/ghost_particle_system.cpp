//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "object/ghost_particle_system.hpp"

#include <algorithm>
#include <math.h>

#include "math/random.hpp"
#include "supertux/globals.hpp"
#include "video/surface.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

//FIXME: Sometimes both ghosts have the same image
//       Ghosts don't change their movement pattern - not random.
GhostParticleSystem::GhostParticleSystem()
{
  init();
}

GhostParticleSystem::GhostParticleSystem(const ReaderMapping& reader) :
  ParticleSystem(reader)
{
  init();
}

GhostParticleSystem::~GhostParticleSystem()
{
}

void
GhostParticleSystem::init()
{
  ghosts[0] = Surface::from_file("images/particles/ghost0.png");
  ghosts[1] = Surface::from_file("images/particles/ghost1.png");

  virtual_width = static_cast<float>(SCREEN_WIDTH) * 2.0f;

  // Create two ghosts.
  size_t ghostcount = 2;
  for (size_t i=0; i<ghostcount; ++i) {
    auto particle = std::make_unique<GhostParticle>();
    particle->pos.x = graphicsRandom.randf(virtual_width);
    particle->pos.y = graphicsRandom.randf(static_cast<float>(SCREEN_HEIGHT));
    int size = graphicsRandom.rand(2);
    particle->texture = ghosts[size];
    particle->speed = graphicsRandom.randf(std::max(50.0f, static_cast<float>(size) * 10.0f),
                                           180.0f + static_cast<float>(size) * 10.0f);
    particles.push_back(std::move(particle));
  }
}

void
GhostParticleSystem::update(float dt_sec)
{
  if (!enabled)
    return;

  for (const auto& part : particles) {
    const auto& particle = dynamic_cast<GhostParticle*>(part.get());
    particle->pos.y -= particle->speed * dt_sec;
    particle->pos.x -= particle->speed * dt_sec;
    if (particle->pos.y > static_cast<float>(SCREEN_HEIGHT)) {
      particle->pos.y = fmodf(particle->pos.y , virtual_height);
      particle->pos.x = graphicsRandom.randf(virtual_width);
    }
  }
}

/* EOF */
