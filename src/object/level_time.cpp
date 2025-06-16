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

#include "object/level_time.hpp"

#include <algorithm>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "editor/editor.hpp"
#include "object/player.hpp"
#include "supertux/debug.hpp"
#include "supertux/game_session.hpp"
#include "supertux/resources.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"

/** When to alert player they're low on time! */
static const float TIME_WARNING = 20;

LevelTime::LevelTime(const ReaderMapping& reader) :
  LayerObject(reader),
  time_surface(Surface::from_file("images/engine/hud/time-0.png")),
  running(!Editor::is_active()),
  time_left()
{
  reader.get("time", time_left, 0.0f);
  if (time_left <= 0 && !Editor::is_active()) {
    log_warning << "No or invalid leveltime specified." << std::endl;
    remove_me();
  }
}

ObjectSettings
LevelTime::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_float(_("Time"), &time_left, "time");
  result.add_remove();

  return result;
}

void
LevelTime::update(float dt_sec)
{
  if (!running) return;

  int players_alive = Sector::current() ? Sector::current()->get_object_count<Player>([](const Player& p) {
    return p.is_active();
  }) : 0;

  if (!players_alive)
    return;

  int prev_time = static_cast<int>(floorf(time_left*5));
  time_left -= dt_sec;
  if (time_left <= 0) {
    // Needed to avoid charging a player coins if they had a checkpoint
    if (GameSession::current())
      GameSession::current()->clear_respawn_points();

    if (time_left <= -5 || !Sector::get().get_players()[0]->get_coins())
    {
      for (auto& p : Sector::get().get_players())
      {
        p->kill(true);
      }
      stop();
    }

    if (prev_time != static_cast<int>(floorf(time_left*5)))
    {
      for (auto& p : Sector::get().get_players())
      {
        if (!p->is_active())
          continue;

        p->add_coins(-1);
        // FIXME: Find a cleaner way to handle this
        //        (Remove only one coin per second, not per player per second)
        break;
      }
    }
  }
}

void
LevelTime::draw(DrawingContext& context)
{
  if (g_debug.hide_player_hud || Editor::is_active())
    return;

  context.push_transform();
  context.set_translation(Vector(0, 0));
  context.transform().scale = 1.f;

  if ((time_left > TIME_WARNING) || (int(g_game_time * 2.5f) % 2)) {
    std::stringstream ss;
    ss << int(time_left);
    std::string time_text = ss.str();

    if (time_surface)
    {
      float all_width = static_cast<float>(time_surface->get_width()) + Resources::normal_font->get_text_width(time_text);
      context.color().draw_surface(time_surface,
                                   Vector((context.get_width() - all_width) / 2.0f,
                                          BORDER_Y + 1),
                                   LAYER_HUD);
      context.color().draw_text(Resources::normal_font, time_text,
                                Vector((context.get_width() - all_width) / 2.0f + static_cast<float>(time_surface->get_width()),
                                       BORDER_Y + 14),
                                ALIGN_LEFT, LAYER_HUD, LevelTime::text_color);
    }
  }

  context.pop_transform();
}

void
LevelTime::start()
{
  running = true;
}

void
LevelTime::stop()
{
  running = false;
}

float
LevelTime::get_time() const
{
  return time_left;
}

void
LevelTime::set_time(float time_left_)
{
  time_left = std::min(std::max(time_left_, 0.0f), 999.0f);
}


void
LevelTime::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<LevelTime>("LevelTime", vm.findClass("GameObject"));

  cls.addFunc("start", &LevelTime::start);
  cls.addFunc("stop", &LevelTime::stop);
  cls.addFunc("get_time", &LevelTime::get_time);
  cls.addFunc("set_time", &LevelTime::set_time);

  cls.addVar("time", &LevelTime::get_time, &LevelTime::set_time);
}

/* EOF */
