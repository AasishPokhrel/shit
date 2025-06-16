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

#include "trigger/scripttrigger.hpp"

#include "editor/editor.hpp"
#include "supertux/debug.hpp"
#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

ScriptTrigger::ScriptTrigger(const ReaderMapping& reader) :
  Trigger(reader),
  triggerevent(),
  script(),
  must_activate(false),
  oneshot(false),
  runcount(0)
{
  reader.get("script", script);
  reader.get("button", must_activate);
  reader.get("oneshot", oneshot);
  if (script.empty() && !Editor::is_active())
    log_warning << "No script set in script trigger" << std::endl;

  if (must_activate)
    triggerevent = EVENT_ACTIVATE;
  else
    triggerevent = EVENT_TOUCH;
}

ObjectSettings
ScriptTrigger::get_settings()
{
  ObjectSettings result = Trigger::get_settings();

  result.add_script(_("Script"), &script, "script");
  result.add_bool(_("Button"), &must_activate, "button");
  result.add_bool(_("Oneshot"), &oneshot, "oneshot", false);

  result.reorder({"script", "button", "width", "height", "x", "y"});

  return result;
}

void
ScriptTrigger::event(Player& , EventType type)
{
  if (type != triggerevent || (oneshot && runcount >= 1))
    return;

  Sector::get().run_script(script, "ScriptTrigger");
  runcount++;
}

void
ScriptTrigger::draw(DrawingContext& context)
{
  if (Editor::is_active() || g_debug.show_collision_rects)
    context.color().draw_filled_rect(m_col.m_bbox, Color(1.0f, 0.0f, 1.0f, 0.6f),
                             0.0f, LAYER_OBJECTS);
}

/* EOF */
