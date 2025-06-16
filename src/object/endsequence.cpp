//  SuperTux - End Sequence
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

#include "object/endsequence.hpp"

#include "object/player.hpp"
#include "supertux/sector.hpp"

EndSequence::EndSequence() :
  m_is_running(false),
  m_is_done(false),
  m_tux_is_stopped(),
  m_end_sequence_controllers()
{
}

EndSequence::~EndSequence()
{
}

void
EndSequence::update(float dt_sec)
{
  if (!m_is_running) return;
  running(dt_sec);
}

void
EndSequence::draw(DrawingContext& /*context*/)
{
}

void
EndSequence::start()
{
  if (m_is_running) return;
  m_is_running = true;
  m_is_done = false;

  starting();
}

void
EndSequence::stop_tux(int player)
{
  m_tux_is_stopped[player] = true;
}

void
EndSequence::stop()
{
  if (!m_is_running) return;
  m_is_running = false;
  m_is_done = true;
  stopping();
}

bool
EndSequence::is_running() const
{
  return m_is_running;
}

bool
EndSequence::is_tux_stopped(int player)
{
  return m_tux_is_stopped[player];
}

bool
EndSequence::is_done() const
{
  return m_is_done;
}

void
EndSequence::starting()
{
}

void
EndSequence::running(float /*dt_sec*/)
{
  for (auto& ctrl : m_end_sequence_controllers)
  {
    ctrl.second->update();
  }
}

void
EndSequence::stopping()
{
}

const Controller*
EndSequence::get_controller(int player)
{
  return get_code_controller(player);
}

CodeController*
EndSequence::get_code_controller(int player)
{
  auto& ctrl = m_end_sequence_controllers[player];

  if (!ctrl)
    ctrl.reset(new CodeController());

  return ctrl.get();
}

/* EOF */
