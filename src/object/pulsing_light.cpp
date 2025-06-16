//  SuperTux - Pulsing Light
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "object/pulsing_light.hpp"

#include <assert.h>
#include <cmath>

#include "object/tilemap.hpp"
#include "math/random.hpp"
#include "math/util.hpp"

PulsingLight::PulsingLight(const Vector& center, float cycle_len_, float min_alpha_, float max_alpha_, const Color& color_, const TileMap* parent_tilemap_) :
  Light(center, color_),
  min_alpha(min_alpha_),
  max_alpha(max_alpha_),
  cycle_len(cycle_len_),
  parent_tilemap(parent_tilemap_),
  rel_position(center),
  rel_color(color_),
  t(0)
{
  assert(cycle_len > 0);

  // start with random phase offset
  t = graphicsRandom.randf(0.0, cycle_len);
}

PulsingLight::~PulsingLight()
{
}

void
PulsingLight::update(float dt_sec)
{
  Light::update(dt_sec);

  t += dt_sec;
  if (t > cycle_len) t -= cycle_len;
}

void
PulsingLight::draw(DrawingContext& context)
{ 
  const float alpha = min_alpha + ((max_alpha - min_alpha) * std::cos(math::TAU * t / cycle_len));
  if (parent_tilemap) {
    position = rel_position + parent_tilemap->get_offset();
    color = (rel_color * parent_tilemap->get_current_tint()).validate();
    color.alpha *= alpha * parent_tilemap->get_alpha();
  } else {
    color.alpha = rel_color.alpha * alpha;
  }
  
  Light::draw(context);
}

/* EOF */
