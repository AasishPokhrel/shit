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

#include "badguy/yeti_stalactite.hpp"

#include "editor/editor.hpp"
#include "sprite/sprite.hpp"

static const float YT_SHAKE_TIME = .8f;

YetiStalactite::YetiStalactite(const ReaderMapping& mapping) :
  Stalactite(mapping)
{
}

void
YetiStalactite::start_shaking()
{
  timer.start(YT_SHAKE_TIME);
  state = STALACTITE_SHAKING;
  if ((static_cast<int>(get_pos().x) / 32) % 2 == 0) {
    m_physic.set_velocity_y(100);
  }
}

bool
YetiStalactite::is_hanging() const
{
  return state == STALACTITE_HANGING;
}

void
YetiStalactite::active_update(float dt_sec)
{
  if (is_hanging() && !m_sticky)
    return;

  Stalactite::active_update(dt_sec);
}

void
YetiStalactite::after_editor_set()
{
  auto target_action = "yeti-stalactite";
  if (m_sprite->has_action(target_action) &&
      m_sprite->get_action() != target_action)
  {
    set_action(target_action);
  }
}

void
YetiStalactite::update(float dt_sec)
{
  // Respawn instead of removing once squished.
  if (get_state() == STATE_SQUISHED && check_state_timer()) {
    set_state(STATE_ACTIVE);
    state = STALACTITE_HANGING;

    // Attempt to minimize any potential collisions during this process.
    set_action("default");
    set_pos(m_start_position);
    m_physic.set_velocity(0, 0);
    set_colgroup_active(COLGROUP_TOUCHABLE);
  }

  // Invoke the badguy's update function to handle regular tasks.
  BadGuy::update(dt_sec);
}

void
YetiStalactite::draw(DrawingContext& context)
{
  if (Editor::is_active())
  {
    BadGuy::draw(context);
  }
  else
  {
    Stalactite::draw(context);
  }
}

bool
YetiStalactite::is_flammable() const
{
  return false;
}

std::vector<Direction>
YetiStalactite::get_allowed_directions() const
{
  return {};
}

/* EOF */
