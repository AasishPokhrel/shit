//  SuperTux - PushButton running a script
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

#include "object/pushbutton.hpp"

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "object/bigsnowball.hpp"
#include "object/player.hpp"
#include "object/rock.hpp"
#include "sprite/sprite.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

namespace {
const std::string BUTTON_SOUND = "sounds/switch.ogg";
//14 -> 8
}

PushButton::PushButton(const ReaderMapping& mapping) :
  StickyObject(mapping, "images/objects/pushbutton/pushbutton.sprite", LAYER_BACKGROUNDTILES+1, COLGROUP_STATIC),
  m_script(),
  m_state(OFF),
  m_dir(Direction::UP)
{
  SoundManager::current()->preload(BUTTON_SOUND);

  if (!mapping.get("script", m_script) && !Editor::is_active())
  {
    log_warning << "No script set for pushbutton." << std::endl;
  }

  bool old_upside_down;
  std::string dir_str;

  if (mapping.get("direction", dir_str))
    m_dir = string_to_dir(dir_str);
  else if (mapping.get("upside-down", old_upside_down) && old_upside_down)
    m_dir = Direction::DOWN;

  mapping.get("sticky", m_sticky, false);

  set_action("off", m_dir, -1);
}

ObjectSettings
PushButton::get_settings()
{
  ObjectSettings result = StickyObject::get_settings();

  result.add_direction(_("Direction"), &m_dir, { Direction::UP, Direction::DOWN }, "direction");
  result.add_script(_("Script"), &m_script, "script");

  result.reorder({"direction", "script", "sticky", "x", "y"});

  return result;
}

void
PushButton::after_editor_set()
{
  MovingSprite::after_editor_set();
  set_action("off", m_dir);
}

void
PushButton::update(float dt_sec)
{
  if (m_sticky) {
    StickyObject::update(dt_sec);
  }
}

HitResponse
PushButton::collision(MovingObject& other, const CollisionHit& hit)
{
  auto player = dynamic_cast<Player*>(&other);
  auto rock = dynamic_cast<Rock*>(&other);
  auto bs = dynamic_cast<BigSnowball*>(&other);

  if (!player && !rock && !bs)
    return FORCE_MOVE;
	if (player)
  {
    float vy = player->get_physic().get_velocity_y();

    if (m_dir == Direction::DOWN)
    {
      if (vy >= 0)
        return FORCE_MOVE;

      if (hit.bottom)
        player->get_physic().set_velocity_y(0);
    }
    else
    {
      if (vy <= 0)
        return FORCE_MOVE;

      if (hit.top)
      {
        player->get_physic().set_velocity_y(0);
        if(!player->is_swimming())
        {
          player->set_on_ground(true);
        }
      }
    }
	}

  if (m_state != OFF || !(m_dir == Direction::DOWN ? hit.bottom : hit.top))
    return FORCE_MOVE;

  // change appearance
  m_state = ON;
  float old_bbox_height = m_col.m_bbox.get_height();
  set_action("on", m_dir, -1);
  float new_bbox_height = m_col.m_bbox.get_height();
  Vector delta(0, old_bbox_height - new_bbox_height);
  set_pos(get_pos() + delta * (m_dir == Direction::DOWN ? 0 : 1.f));

  // play sound
  SoundManager::current()->play(BUTTON_SOUND, get_pos());

  // run script
  Sector::get().run_script(m_script, "PushButton");

  return FORCE_MOVE;
}

void
PushButton::on_flip(float height)
{
  MovingSprite::on_flip(height);
  m_dir = m_dir == Direction::UP ? Direction::DOWN : Direction::UP;
  set_action(m_state == OFF ? "off" : "on", m_dir);
}

/* EOF */
