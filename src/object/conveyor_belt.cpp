//  SuperTux
//  Copyright (C) 2022 Raoul1808 <raoulthegeek@gmail.com>
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

#include "object/conveyor_belt.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "badguy/walking_badguy.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/direction.hpp"
#include "math/util.hpp"
#include "util/reader_mapping.hpp"

ConveyorBelt::ConveyorBelt(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/conveyor_belt/conveyor.sprite", LAYER_TILES),
  m_running(true),
  m_dir(Direction::LEFT),
  m_length(1),
  m_speed(1.0f),
  m_frame(0.0f),
  m_frame_index(0)
{
  set_group(COLGROUP_STATIC);
  reader.get("running", m_running);
  std::string dir_str;
  if (reader.get("direction", dir_str))
    m_dir = string_to_dir(dir_str);

  reader.get("speed", m_speed);
  m_speed = math::clamp(m_speed, 0.0f, MAX_SPEED);

  reader.get("length", m_length, 1);
  if (m_length <= 0)
    m_length = 1;

  if (!m_running)
    set_action("stopped");
  else
    set_action(m_dir);
}

ObjectSettings
ConveyorBelt::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_direction(_("Direction"), &m_dir, { Direction::LEFT, Direction::RIGHT }, "direction");
  result.add_float(_("Speed"), &m_speed, "speed", 1.0f);
  result.add_bool(_("Running"), &m_running, "running", true);
  result.add_int(_("Length"), &m_length, "length", 3);

  result.reorder({"running", "name", "direction", "fast", "length", "sprite", "x", "y"});

  return result;
}

HitResponse
ConveyorBelt::collision(MovingObject& other, const CollisionHit& hit)
{
  WalkingBadguy* walking_badguy = dynamic_cast<WalkingBadguy*>(&other);
  if (walking_badguy)
    walking_badguy->override_stay_on_platform();

  return FORCE_MOVE;
}

void
ConveyorBelt::update(float dt_sec)
{
  if (m_running)
  {
    Vector shift_movement(m_speed * (m_dir == Direction::LEFT ? -1.0f : 1.0f) * 32.0f * dt_sec, 0.0f);
    m_col.propagate_movement(shift_movement);

    int frames = m_sprite->get_frames();

    m_frame += m_speed * static_cast<float>(frames) * dt_sec * 2.5f;

    while (m_frame >= 1.0f)
    {
      m_frame -= 1.0f;
      m_frame_index++;
    }

    while (m_frame_index >= frames)
    {
      m_frame_index -= frames;
    }
  }
}

void
ConveyorBelt::draw(DrawingContext &context)
{
  int frame_index = m_running ? m_frame_index : 0;
  for (int i = 0; i < m_length; i++)
  {
    m_sprite->set_frame(frame_index);
    m_sprite->draw(context.color(), get_pos() + Vector(static_cast<float>(i) * 32.f, 0.f), m_layer);
  }
}

void
ConveyorBelt::update_hitbox()
{
  MovingSprite::update_hitbox();

  m_col.m_bbox.set_size(m_sprite->get_current_hitbox_width() * static_cast<float>(m_length),
                        m_sprite->get_current_hitbox_height());
}

void
ConveyorBelt::after_editor_set()
{
  MovingSprite::after_editor_set();

  if (m_length <= 0)
    m_length = 1;
  set_action(m_dir);
}

void
ConveyorBelt::start()
{
  m_running = true;
  set_action(m_dir);
}

void
ConveyorBelt::stop()
{
  m_running = false;
  set_action("stopped");
}

void
ConveyorBelt::move_left()
{
  m_dir = Direction::LEFT;
  if (m_running)
    set_action("left");
}

void
ConveyorBelt::move_right()
{
  m_dir = Direction::RIGHT;
  if (m_running)
    set_action("right");
}

void
ConveyorBelt::set_speed(float target_speed)
{
  m_speed = math::clamp(target_speed, 0.0f, MAX_SPEED);
}


void
ConveyorBelt::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<ConveyorBelt>("ConveyorBelt", vm.findClass("MovingSprite"));

  cls.addFunc("start", &ConveyorBelt::start);
  cls.addFunc("stop", &ConveyorBelt::stop);
  cls.addFunc("move_left", &ConveyorBelt::move_left);
  cls.addFunc("move_right", &ConveyorBelt::move_right);
  cls.addFunc("set_speed", &ConveyorBelt::set_speed);
  cls.addFunc("get_speed", &ConveyorBelt::get_speed);

  cls.addVar("speed", &ConveyorBelt::get_speed, &ConveyorBelt::set_speed);
}

/* EOF */
