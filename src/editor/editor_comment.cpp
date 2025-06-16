//  SuperTux
//  Copyright (C) 2025 MatusGuy <matusguy@supertux.org>
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

#include "editor_comment.hpp"

#include "editor/editor.hpp"
#include "supertux/info_box_line.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

EditorComment::EditorComment(const ReaderMapping& reader):
  MovingObject(reader),
  m_comment(""),
  m_lines()
{
  parse_type(reader);

  float w,h;
  reader.get("width", w, 32.f * 3);
  reader.get("height", h, 32.f * 3);
  m_col.m_bbox.set_size(w, h);

  reader.get("comment", m_comment);
  refresh_comment();
}

void
EditorComment::draw(DrawingContext& context)
{
  // TODO: There should be an object factory param
  // for objects that only exist when testing in editor.
  if (!Editor::current())
    return;

  float y = get_y();
  for (std::unique_ptr<InfoBoxLine>& line : m_lines)
  {
    if (y >= get_bbox().get_bottom())
      break;

    line->draw(context, Rectf(Vector(get_x(), y), Sizef(get_width() / 2, y)), get_layer() + 1, InfoBoxLine::LEFT);
    y += line->get_height();
  }

  context.color().draw_filled_rect(get_bbox(), get_color(), 0.f, get_layer());
}

void
EditorComment::check_state()
{
  refresh_comment();
}

GameObjectTypes
EditorComment::get_types() const
{
  return {
    /* l10n: A note refers to a reminder left for future
       level designers to read in order to better understand
       the usage of certain features in a level, for example. */
    { "note", _("Note") },

    /* l10n: A to-do refers to a reminder left for the
       author of the comment to finish designing something in
       a level. */
    { "todo", _("To-do") },

    /* l10n: A fix-me refers to a reminder left for the
       author of the comment to fix a flaw with the level
       design. */
    { "fixme", _("Fix-me") },

    /* l10n: A hack refers to usage of an unintended or
       non-ideal way of implementing a level design idea. */
    { "hack", _("Hack") }
  };
}

ObjectSettings
EditorComment::get_settings()
{
  ObjectSettings result = MovingObject::get_settings();

  result.add_multiline_text(_("Comment"), &m_comment, "comment");

  return result;
}

void
EditorComment::refresh_comment()
{
  m_lines = InfoBoxLine::split(m_comment, get_width(), true);
}

Color
EditorComment::get_color() const
{
  switch (m_type)
  {
    case NOTE: return Color(0.19f, 0.65f, 0.32f, 0.6f);
    case TODO: return Color(0.26f, 0.53f, 0.96f, 0.6f);
    case FIXME: return Color(1.f, 0.44f, 0.11f, 0.6f);
    case HACK: return Color(0.96f, 0.23f, 0.23f, 0.6f);
    default: return Color();
  }
}

/* EOF */
