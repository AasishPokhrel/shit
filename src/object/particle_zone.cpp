//  SuperTux - Particle spawn zone
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

#include "object/particle_zone.hpp"

#include "editor/editor.hpp"
#include "supertux/resources.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

ParticleZone::ParticleZone(const ReaderMapping& reader) :
  MovingObject(reader),
  m_enabled(),
  m_particle_name()
{
  parse_type(reader);

  float w,h;
  reader.get("x", m_col.m_bbox.get_left(), 0.0f);
  reader.get("y", m_col.m_bbox.get_top(), 0.0f);
  reader.get("width", w, 32.0f);
  reader.get("height", h, 32.0f);
  m_col.m_bbox.set_size(w, h);

  reader.get("enabled", m_enabled, true);
  reader.get("particle-name", m_particle_name, "");

  set_group(COLGROUP_TOUCHABLE);
}

ObjectSettings
ParticleZone::get_settings()
{
  ObjectSettings result = MovingObject::get_settings();

  result.add_bool(_("Enabled"), &m_enabled, "enabled", true);
  result.add_text(_("Particle Name"), &m_particle_name, "particle-name");

  result.reorder({"width", "height", "name", "x", "y"});

  return result;
}

GameObjectTypes
ParticleZone::get_types() const
{
  return {
    { "spawn", _("Spawn") },
    { "life", _("Life zone") },
    { "life-clear", _("Life zone (clear)") },
    { "killer", _("Kill particles") },
    { "destroyer", _("Clear particles") }
  };
}

void
ParticleZone::update(float dt_sec)
{
  // This object doesn't manage creating particles :)
  // See `src/object/custom_particle_system.*pp` for that
}

void
ParticleZone::draw(DrawingContext& context)
{
  if (Editor::is_active()) {
    Color c;
    switch(m_type) {
    case ParticleZoneType::Spawn:
      c = Color(0.5f, 0.5f, 1.0f, 0.6f);
      break;
    case ParticleZoneType::Life:
      c = Color(0.5f, 1.0f, 0.5f, 0.6f);
      break;
    case ParticleZoneType::LifeClear:
      c = Color(1.0f, 1.0f, 0.5f, 0.6f);
      break;
    case ParticleZoneType::Killer:
      c = Color(1.0f, 0.75f, 0.5f, 0.6f);
      break;
    case ParticleZoneType::Destroyer:
      c = Color(1.0f, 0.5f, 0.5f, 0.6f);
      break;
    }

    context.color().draw_filled_rect(m_col.m_bbox, c,
                           0.0f, LAYER_OBJECTS);
    context.color().draw_text(Resources::small_font,
                          m_particle_name, 
                          m_col.m_bbox.p1(),
                          FontAlignment::ALIGN_LEFT,
                          LAYER_OBJECTS,
                          Color::WHITE);
  }
}

HitResponse
ParticleZone::collision(MovingObject& other, const CollisionHit& hit)
{
  return ABORT_MOVE;
}

/* EOF */
