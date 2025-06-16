//  SuperTux
//  Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#include "badguy/owl.hpp"

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "sprite/sprite.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

namespace {

const float FLYING_SPEED = 120.0f;
const float ACTIVATION_DISTANCE = 128.0f;

} // namespace

std::vector<std::string> Owl::s_portable_objects;

Owl::Owl(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/owl/owl.sprite", LAYER_OBJECTS + 1),
  carried_obj_name(),
  carried_object(nullptr)
{
  reader.get("carry", carried_obj_name, "skydive");
  set_action("fly", m_dir);
  if (Editor::is_active() && s_portable_objects.empty())
    s_portable_objects = GameObjectFactory::instance().get_registered_objects(ObjectFactory::OBJ_PARAM_PORTABLE);
}

void
Owl::initialize()
{
  m_physic.set_velocity_x(m_dir == Direction::LEFT ? -FLYING_SPEED : FLYING_SPEED);
  m_physic.enable_gravity(false);

  // If we add the carried object to the sector while we're editing
  // a level with the editor, it gets written to the level file,
  // resulting in two carried objects. Returning early is much better.
  if (Editor::is_active())
  {
    return;
  }

  auto game_object = GameObjectFactory::instance().create(carried_obj_name, get_pos(), m_dir);
  if (game_object == nullptr)
  {
    log_fatal << "Creating \"" << carried_obj_name << "\" object failed." << std::endl;
  }
  else
  {
    carried_object = dynamic_cast<Portable*>(game_object.get());
    if (carried_object == nullptr)
    {
      log_warning << "Object is not portable: " << carried_obj_name << std::endl;
    }
    else
    {
      carried_object->grab(*this, get_pos(), m_dir);
      Sector::get().add_object(std::move(game_object));
    }
  }
}

bool
Owl::is_above_player() const
{
  auto player = Sector::get().get_nearest_player(m_col.m_bbox);
  if (!player)
    return false;

  // Let go of carried objects a short while *before* Tux is below us. This
  // makes it more likely that we'll hit him.
  float x_offset = (m_dir == Direction::LEFT) ? ACTIVATION_DISTANCE : -ACTIVATION_DISTANCE;

  const Rectf& player_bbox = player->get_bbox();

  return ((player_bbox.get_top() >= m_col.m_bbox.get_bottom()) /* player is below us */
          && ((player_bbox.get_right() + x_offset) > m_col.m_bbox.get_left())
          && ((player_bbox.get_left() + x_offset) < m_col.m_bbox.get_right()));
}

void
Owl::active_update (float dt_sec)
{
  BadGuy::active_update (dt_sec);

  if (m_frozen)
    return;

  if (carried_object)
  {
    set_action("carry", m_dir);

    if (!is_above_player ()) {
      Vector obj_pos = get_anchor_pos(m_col.m_bbox, ANCHOR_BOTTOM);
      auto obj = dynamic_cast<MovingObject*>(carried_object);
      auto verticalOffset = obj != nullptr ? obj->get_bbox().get_width() / 2.f : 16.f;
      obj_pos.x -= verticalOffset;
      obj_pos.y += 3.f; /* Move a little away from the hitbox (the body). Looks nicer. */

      // Drop carried object before leaving the screen
      if (obj_pos.x<=16 || obj_pos.x+16>=Sector::get().get_width()){
        carried_object->ungrab (*this, m_dir);
        carried_object = nullptr;
      }

     else
        carried_object->grab (*this, obj_pos, m_dir);
    }
    else { /* if (is_above_player) */
      ungrab_carried_object();
    }
  }
  else /* if (carried_object) */
  {
    set_action("fly", m_dir);
  }
}

bool
Owl::collision_squished(MovingObject& object)
{
  if (m_frozen)
    return BadGuy::collision_squished(object);

  auto player = Sector::get().get_nearest_player(m_col.m_bbox);
  if (player)
    player->bounce (*this);

  ungrab_carried_object();

  kill_fall ();
  return true;
}

void
Owl::kill_fall()
{
  if (!m_frozen)
  {
    SoundManager::current()->play("sounds/fall.wav", get_pos());
    m_physic.set_velocity_y(0);
    m_physic.set_acceleration_y(0);
    m_physic.enable_gravity(true);
    set_state(STATE_FALLING);
  }
  else
    BadGuy::kill_fall();

  ungrab_carried_object();

  // Start the dead-script.
  run_dead_script();
}

void
Owl::freeze()
{
  ungrab_carried_object();
  m_physic.enable_gravity(true);
  BadGuy::freeze();
}

void
Owl::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
  m_physic.set_velocity_x(m_dir == Direction::LEFT ? -FLYING_SPEED : FLYING_SPEED);
  m_physic.enable_gravity(false);
}

bool
Owl::is_freezable() const
{
  return true;
}

void
Owl::collision_solid(const CollisionHit& hit)
{
  if (m_frozen)
  {
    BadGuy::collision_solid(hit);
    return;
  }
  if (hit.top || hit.bottom) {
    m_physic.set_velocity_y(0);
  } else if (hit.left || hit.right) {
    if (m_dir == Direction::LEFT) {
      m_dir = Direction::RIGHT;
      m_physic.set_velocity_x (FLYING_SPEED);
    }
    else {
      m_dir = Direction::LEFT;
      m_physic.set_velocity_x (-FLYING_SPEED);
    }
  }
}

void
Owl::ignite()
{
  ungrab_carried_object();
  BadGuy::ignite();
}

void
Owl::ungrab_carried_object()
{
  if (carried_object != nullptr) {
    carried_object->ungrab (*this, m_dir);
    carried_object = nullptr;
  }
}

ObjectSettings
Owl::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_list(_("Carry"), "carry", s_portable_objects, &carried_obj_name);

  result.reorder({"carry", "direction", "sprite", "x", "y"});

  return result;
}

/* EOF */
