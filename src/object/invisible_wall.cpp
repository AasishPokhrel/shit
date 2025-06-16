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

#include "object/invisible_wall.hpp"

#include "editor/editor.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

InvisibleWall::InvisibleWall(const ReaderMapping& mapping):
  MovingObject(mapping),
  width(),
  height()
{
  mapping.get("x", m_col.m_bbox.get_left(), 0.0f);
  mapping.get("y", m_col.m_bbox.get_top(), 0.0f);
  mapping.get("width", width, 32.0f);
  mapping.get("height", height, 32.0f);

  m_col.m_bbox.set_size(width, height);

  m_col.m_group = COLGROUP_STATIC;
}

ObjectSettings
InvisibleWall::get_settings()
{
  width = m_col.m_bbox.get_width();
  height = m_col.m_bbox.get_height();

  ObjectSettings result = MovingObject::get_settings();

  //result.add_float(_("Width"), &width, "width");
  //result.add_float(_("Height"), &height, "height");

  return result;
}

void
InvisibleWall::after_editor_set() {
  m_col.m_bbox.set_size(width, height);
}

HitResponse
InvisibleWall::collision(MovingObject& , const CollisionHit& )
{
  return FORCE_MOVE;
}

void
InvisibleWall::draw(DrawingContext& context)
{
  if (Editor::is_active()) {
    context.color().draw_filled_rect(m_col.m_bbox, Color(0.0f, 0.0f, 0.0f, 0.6f),
                             0.0f, LAYER_OBJECTS);
  }
}

void
InvisibleWall::update(float )
{
}

/* EOF */
