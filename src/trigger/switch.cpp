//  SuperTux - Switch Trigger
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#include "trigger/switch.hpp"

#include <sstream>

#include "audio/sound_manager.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"

namespace {
  const std::string SWITCH_SOUND = "sounds/switch.ogg";
} // namespace

Switch::Switch(const ReaderMapping& reader) :
  StickyTrigger(reader, "images/objects/switch/switch.sprite"),
  m_script(),
  m_off_script(),
  m_state(OFF),
  m_bistable(),
  m_dir(Direction::NONE)
{
  std::string dir_str;
  if (reader.get("direction", dir_str))
    m_dir = string_to_dir(dir_str);
  else
    m_dir = Direction::NONE;

  set_action("off", m_dir);

  reader.get("script", m_script);
  reader.get("sticky", m_sticky, false);
  m_bistable = reader.get("off-script", m_off_script);

  SoundManager::current()->preload(SWITCH_SOUND);
}

Switch::~Switch()
{
}

ObjectSettings
Switch::get_settings()
{
  ObjectSettings result = StickyTrigger::get_settings();

  result.add_direction(_("Direction"), &m_dir,
                        { Direction::NONE, Direction::LEFT, Direction::RIGHT, Direction::UP, Direction::DOWN }, "direction");

  result.add_script(_("Turn on script"), &m_script, "script");
  result.add_script(_("Turn off script"), &m_off_script, "off-script");

  result.reorder({"direction", "script", "off-script", "sticky", "sprite", "x", "y"});

  return result;
}

void
Switch::update(float dt_sec)
{
  if (m_sticky && m_dir != Direction::NONE)
    StickyObject::update(dt_sec);

  switch (m_state) {
    case OFF:
      break;
    case TURN_ON:
      if (m_sprite->animation_done()) {
        std::ostringstream location;
        location << "switch" << m_col.m_bbox.p1();
        Sector::get().run_script(m_script, location.str());

        set_action("on", m_dir, 1);
        m_state = ON;
      }
      break;
    case ON:
      if (m_sprite->animation_done() && !m_bistable) {
        set_action("turnoff", m_dir, 1);
        m_state = TURN_OFF;
      }
      break;
    case TURN_OFF:
      if (m_sprite->animation_done()) {
        if (m_bistable) {
          std::ostringstream location;
          location << "switch" << m_col.m_bbox.p1();
          Sector::get().run_script(m_off_script, location.str());
        }

        set_action("off", m_dir);
        m_state = OFF;
      }
      break;
  }
}

void
Switch::event(Player& , EventType type)
{
  if (type != EVENT_ACTIVATE) return;

  switch (m_state) {
    case OFF:
      set_action("turnon", m_dir, 1);
      SoundManager::current()->play(SWITCH_SOUND, get_pos());
      m_state = TURN_ON;
      break;
    case TURN_ON:
      break;
    case ON:
      if (m_bistable) {
        set_action("turnoff", m_dir, 1);
        SoundManager::current()->play(SWITCH_SOUND, get_pos());
        m_state = TURN_OFF;
      }
      break;
    case TURN_OFF:
      break;
  }
}

void
Switch::after_editor_set()
{
  StickyTrigger::after_editor_set();

  set_action("off", m_dir);
}

void
Switch::on_flip(float height)
{
  StickyTrigger::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

/* EOF */
