//  SuperTux - End Sequence: Tux walks right
//  Copyright (C) 2007 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "object/endsequence_walk.hpp"

#include "object/player.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"

EndSequenceWalk::EndSequenceWalk() :
  EndSequence(),
  last_x_pos(),
  endsequence_timer()
{
}

EndSequenceWalk::~EndSequenceWalk()
{
}

void
EndSequenceWalk::draw(DrawingContext& /*context*/)
{
}

void
EndSequenceWalk::starting()
{
  EndSequence::starting();
  last_x_pos = -1;
  endsequence_timer.start(7.3f * ScreenManager::current()->get_speed());
}

void
EndSequenceWalk::running(float dt_sec)
{
  EndSequence::running(dt_sec);

  for (auto* player : Sector::get().get_players())
  {
    int dir = player->get_ending_direction();
    if (dir && !m_tux_is_stopped[player->get_id()]) {
      get_code_controller(player->get_id())->press(dir > 0 ? Control::RIGHT : Control::LEFT);
      if (int(last_x_pos) == int(player->get_pos().x)) {
        get_code_controller(player->get_id())->press(Control::JUMP);
      }
      last_x_pos = player->get_pos().x;
    }
  }

  if (endsequence_timer.check()) m_is_done = true;
}

void
EndSequenceWalk::stopping()
{
  EndSequence::stopping();
}

/* EOF */
