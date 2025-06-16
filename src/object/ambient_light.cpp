//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "object/ambient_light.hpp"

#include <sexp/io.hpp>
#include <sexp/value.hpp>

#include "math/util.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "video/drawing_context.hpp"

AmbientLight::AmbientLight(const Color& color) :
  m_ambient_light(color),
  m_ambient_light_fading(false),
  m_source_ambient_light(color),
  m_target_ambient_light(color),
  m_ambient_light_fade_duration(0.0f),
  m_ambient_light_fade_accum(0.0f)
{
}

AmbientLight::AmbientLight(const ReaderMapping& mapping) :
  LayerObject(mapping),
  m_ambient_light(1.0f, 1.0f, 1.0f, 1.0f),
  m_ambient_light_fading(false),
  m_source_ambient_light(1.0f, 1.0f, 1.0f, 1.0f),
  m_target_ambient_light(1.0f, 1.0f, 1.0f, 1.0f),
  m_ambient_light_fade_duration(0.0f),
  m_ambient_light_fade_accum(0.0f)
{
  std::vector<float> color_vec;
  if (mapping.get("color", color_vec))
  {
    if (color_vec.size() < 3) {
      log_warning << "'ambient-light' requires three float arguments" << std::endl;
    } else {
      m_ambient_light = Color(color_vec);
    }
  }
}

void
AmbientLight::update(float dt_sec)
{
  if (m_ambient_light_fading)
  {
    m_ambient_light_fade_accum += dt_sec;
    float percent_done = m_ambient_light_fade_accum / m_ambient_light_fade_duration * 1.0f;
    float r = (1.0f - percent_done) * m_source_ambient_light.red + percent_done * m_target_ambient_light.red;
    float g = (1.0f - percent_done) * m_source_ambient_light.green + percent_done * m_target_ambient_light.green;
    float b = (1.0f - percent_done) * m_source_ambient_light.blue + percent_done * m_target_ambient_light.blue;

    r = math::clamp(r, 0.0f, 1.0f);
    g = math::clamp(g, 0.0f, 1.0f);
    b = math::clamp(b, 0.0f, 1.0f);

    m_ambient_light = Color(r, g, b);

    if (m_ambient_light_fade_accum >= m_ambient_light_fade_duration)
    {
      m_ambient_light = m_target_ambient_light;
      m_ambient_light_fading = false;
      m_ambient_light_fade_accum = 0;
    }
  }
}

void
AmbientLight::draw(DrawingContext& context)
{
  context.set_ambient_color(m_ambient_light);
}

void
AmbientLight::fade_to_ambient_light(float red, float green, float blue, float seconds)
{
  if (seconds == 0)
  {
    m_ambient_light = Color(red, green, blue);
    return;
  }

  m_ambient_light_fading = true;
  m_ambient_light_fade_accum = 0;
  m_ambient_light_fade_duration = seconds;
  m_source_ambient_light = m_ambient_light;
  m_target_ambient_light = Color(red, green, blue);
}

ObjectSettings
AmbientLight::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_color(_("Color"), &m_ambient_light, "color");

  return result;
}

/* EOF */
