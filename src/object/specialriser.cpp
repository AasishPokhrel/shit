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

#include "object/specialriser.hpp"

#include "supertux/sector.hpp"
#include "video/drawing_context.hpp"

SpecialRiser::SpecialRiser(const Vector& pos, std::unique_ptr<MovingObject> child, bool is_solid) :
  m_start_pos(pos),
  m_offset(0),
  m_child(std::move(child))
{
  m_child->set_pos(pos - Vector(0,32));
  set_pos(m_start_pos);
  m_col.m_bbox.set_size(m_child->get_bbox().get_width(), 32);

  // Initial update of child object, in case it's required to be visible.
  // For example, badguys.
  m_child->update(0.f);

  if (is_solid)
    set_group(COLGROUP_STATIC);
  else
    set_group(COLGROUP_DISABLED);
}

void
SpecialRiser::update(float dt_sec)
{
  m_offset += 50 * dt_sec;
  set_pos(m_start_pos - Vector(0, m_offset));
  if (m_offset > 32) {
    Sector::get().add_object(std::move(m_child));
    remove_me();
  }
}

void
SpecialRiser::draw(DrawingContext& context)
{
  context.push_transform();
  context.set_translation(
    context.get_translation() + Vector(0, -32 + m_offset));
  m_child->draw(context);
  context.pop_transform();
}

/* EOF */
