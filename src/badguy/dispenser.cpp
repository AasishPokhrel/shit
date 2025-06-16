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

#include "badguy/dispenser.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "math/random.hpp"
#include "object/bullet.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/sector.hpp"
#include "util/reader_iterator.hpp"
#include "util/reader_mapping.hpp"

Dispenser::Dispenser(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/dispenser/dropper.sprite", LAYER_OBJECTS + 5),
  m_cycle(),
  m_objects(),
  m_next_object(0),
  m_dispense_timer(),
  m_autotarget(false),
  m_random(),
  m_gravity(),
  m_limit_dispensed_badguys(),
  m_max_concurrent_badguys(),
  m_current_badguys()
{
  parse_type(reader);

  SoundManager::current()->preload("sounds/squish.wav");

  reader.get("cycle", m_cycle, 5.0f);

  reader.get("gravity", m_gravity);
  m_physic.enable_gravity(m_gravity);

  reader.get("random", m_random, false);

  std::vector<std::string> badguys;
  if (reader.get("badguy", badguys)) // Backward compatibility.
  {
    for (auto& badguy : badguys)
      add_object(GameObjectFactory::instance().create(badguy));
  }
  else
  {
    std::optional<ReaderMapping> objects_mapping;
    if (reader.get("objects", objects_mapping))
    {
      auto iter = objects_mapping->get_iter();
      while (iter.next())
        add_object(GameObjectFactory::instance().create(iter.get_key(), iter.as_mapping()));
    }
  }

  m_dir = m_start_dir; // Reset direction to default.

  reader.get("limit-dispensed-badguys", m_limit_dispensed_badguys, false);
  reader.get("max-concurrent-badguys", m_max_concurrent_badguys, 0);

//  if (badguys.size() <= 0)
//    throw std::runtime_error("No badguys in dispenser.");

  set_correct_colgroup();
  set_correct_action();
  update_hitbox();
  m_countMe = false;
}

void
Dispenser::add_object(std::unique_ptr<GameObject> object)
{
  auto moving_object = dynamic_cast<MovingObject*>(object.get());
  if (!GameObjectFactory::instance().has_params(object->get_class_name(), ObjectFactory::RegisteredObjectParam::OBJ_PARAM_DISPENSABLE) ||
      !moving_object) // Object is not MovingObject, or is not dispensable.
  {
    log_warning << object->get_class_name() << " is not dispensable. Removing from dispenser object list." << std::endl;
    return;
  }

  moving_object->set_parent_dispenser(this);
  m_objects.push_back(std::move(object));
}

void
Dispenser::draw(DrawingContext& context)
{
  if (m_type != DispenserType::POINT || Editor::is_active())
    BadGuy::draw(context);
}

void
Dispenser::initialize()
{
  m_dir = m_start_dir; // Reset direction to default.
}

void
Dispenser::activate()
{
  m_dispense_timer.start(m_cycle, true);
  launch_object();
}

void
Dispenser::deactivate()
{
  m_dispense_timer.stop();
}

HitResponse
Dispenser::collision(MovingObject& other, const CollisionHit& hit)
{
  auto bullet = dynamic_cast<Bullet*> (&other);
  if (bullet)
    return collision_bullet(*bullet, hit);

  return FORCE_MOVE;
}

void
Dispenser::active_update(float dt_sec)
{
  if (m_gravity)
  {
    BadGuy::active_update(dt_sec);
  }
  if (m_dispense_timer.check())
  {
    auto player = get_nearest_player();
    if (player)
    {
      if(!player->is_alive())
      {
        return;
      }
      
      // Auto always shoots in Tux's direction.
      if (m_autotarget)
      {
        Direction target_dir = (player->get_pos().x > get_pos().x) ? Direction::RIGHT : Direction::LEFT;
        if (m_dir != target_dir)
        {
          m_dir = target_dir;
          return;
        }
      }
    }

    launch_object();
  }

  if (m_frozen)
    set_correct_colgroup();
}

void
Dispenser::kill_fall()
{
}

void
Dispenser::launch_object()
{
  if (m_objects.empty()) return;
  if (m_frozen) return;

  // FIXME: Does is_offscreen() work right here?
  if (!is_offscreen() && !Editor::is_active())
  {
    Direction launch_dir = m_dir;
    if (!m_autotarget && m_start_dir == Direction::AUTO)
    {
      Player* player = get_nearest_player();
      if (player)
        launch_dir = (player->get_pos().x > get_pos().x) ? Direction::RIGHT : Direction::LEFT;
    }

    if (m_objects.size() > 1)
    {
      if (m_random)
      {
        m_next_object = static_cast<unsigned int>(gameRandom.rand(static_cast<int>(m_objects.size())));
      }
      else
      {
        m_next_object++;

        if (m_next_object >= m_objects.size())
          m_next_object = 0;
      }
    }

    auto object = m_objects[m_next_object].get();
    auto obj_badguy = dynamic_cast<BadGuy*>(object);
    if (obj_badguy && m_limit_dispensed_badguys &&
        m_current_badguys >= m_max_concurrent_badguys)
    {
      // The object is a badguy and max concurrent badguys limit has been reached.
      // Do not dispense.
      return;
    }

    try
    {
      auto game_object = GameObjectFactory::instance().create(object->get_class_name(), get_pos(), launch_dir, object->save());
      if (!game_object)
      {
        throw std::runtime_error("Creating " + object->get_class_name() + " object failed.");
      }
      auto moving_object = static_cast<MovingObject*>(game_object.get());

      Rectf object_bbox = moving_object->get_bbox();
      Vector spawnpoint(0.0f, 0.0f);

      switch (m_type)
      {
        case DispenserType::GRANITO:
        case DispenserType::DROPPER:
          if (m_flip == NO_FLIP)
          {
            spawnpoint = get_anchor_pos (m_col.m_bbox, ANCHOR_BOTTOM);
            spawnpoint.x -= 0.5f * object_bbox.get_width();
          }
          else
          {
            spawnpoint = get_anchor_pos (m_col.m_bbox, ANCHOR_TOP);
            spawnpoint.y -= m_col.m_bbox.get_height();
            spawnpoint.x -= 0.5f * object_bbox.get_width();
          }
          break;

        case DispenserType::CANNON:
          spawnpoint = get_pos(); /* Top-left corner of the cannon. */
          if (launch_dir == Direction::LEFT)
            spawnpoint.x -= object_bbox.get_width() + 1;
          else
            spawnpoint.x += m_col.m_bbox.get_width() + 1;
          if (m_flip != NO_FLIP)
            spawnpoint.y += (m_col.m_bbox.get_height() - 20);
          break;

        case DispenserType::POINT:
          spawnpoint = m_col.m_bbox.p1();
          break;

        default:
          break;
      }

      /* Now we set the real spawn position. */
      moving_object->set_pos(spawnpoint);

      /* Set reference to dispenser in the object itself. */
      moving_object->set_parent_dispenser(this);

      if (obj_badguy) // The object is a badguy.
      {
        auto badguy = static_cast<BadGuy*>(moving_object);

        /* We don't want to count dispensed badguys in level stats. */
        badguy->m_countMe = false;

        if (m_limit_dispensed_badguys)
          m_current_badguys++;
      }

      Sector::get().add_object(std::move(game_object));
    }
    catch(const std::exception& e)
    {
      log_warning << "Error dispensing object: " << e.what() << std::endl;
      return;
    }
  }
}

void
Dispenser::freeze()
{
  if (m_type == DispenserType::POINT || m_type == DispenserType::GRANITO)
    return;

  SoundManager::current()->play("sounds/sizzle.ogg", get_pos());
  m_frozen = true;

  const std::string cannon_iced = "iced-" + dir_to_string(m_dir);
  if (m_type == DispenserType::CANNON && m_sprite->has_action(cannon_iced))
  {
    set_action(cannon_iced, 1);
    // When the dispenser is a cannon, it uses the respective "iced" action, based on the current direction.
  }
  else
  {
    if (m_type == DispenserType::DROPPER && m_sprite->has_action("dropper-iced"))
    {
      set_action("dropper-iced", 1);
      // When the dispenser is a dropper, it uses the "dropper-iced".
    }
    else
    {
      m_sprite->set_color(Color(0.6f, 0.72f, 0.88f));
      m_sprite->stop_animation();
      // When the dispenser is something else (unprobable), or has no matching iced sprite, it shades to blue.
    }
  }
  m_dispense_timer.stop();
  m_unfreeze_timer.start(8.f);
}

void
Dispenser::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);

  set_correct_colgroup();
  set_correct_action();
  activate();
}

bool
Dispenser::is_freezable() const
{
  return true;
}

bool
Dispenser::is_flammable() const
{
  return false;
}

bool
Dispenser::is_portable() const
{
  return false;
}

void
Dispenser::set_correct_action()
{
  switch (m_type)
  {
    case DispenserType::GRANITO:
    case DispenserType::CANNON:
      set_action(m_start_dir);
      break;
    case DispenserType::DROPPER:
      set_action("dropper");
      break;
    default:
      break;
  }
}

void Dispenser::set_correct_colgroup()
{
  switch (m_type)
  {
    case DispenserType::GRANITO:
    case DispenserType::POINT:
      set_colgroup_active(COLGROUP_DISABLED);
      break;
    default:
      set_colgroup_active(m_gravity ? COLGROUP_MOVING_STATIC : COLGROUP_STATIC);
      break;
  }
}

void
Dispenser::on_type_change(int old_type)
{
  MovingSprite::on_type_change(old_type);

  if (old_type == GRANITO || m_type == GRANITO)
  {
    m_objects.clear();
    if (m_type == GRANITO) // Switching to type GRANITO
      add_object(GameObjectFactory::instance().create("corrupted_granito"));
  }
}

void
Dispenser::after_editor_set()
{
  BadGuy::after_editor_set();
  set_correct_action();
}

ObjectSettings
Dispenser::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_float(_("Interval (seconds)"), &m_cycle, "cycle");
  result.add_bool(_("Random"), &m_random, "random", false);
  if (m_type != GRANITO)
  {
    result.add_objects(_("Objects"), &m_objects, ObjectFactory::RegisteredObjectParam::OBJ_PARAM_DISPENSABLE,
                       [this](auto obj) { add_object(std::move(obj)); }, "objects");
  }
  result.add_bool(_("Limit dispensed badguys"), &m_limit_dispensed_badguys,
                  "limit-dispensed-badguys", false);
  result.add_bool(_("Obey Gravity"), &m_gravity,
                  "gravity", false);
  result.add_int(_("Max concurrent badguys"), &m_max_concurrent_badguys,
                 "max-concurrent-badguys", 0);

  result.reorder({"cycle", "random", "type", "objects", "direction", "gravity", "limit-dispensed-badguys", "max-concurrent-badguys", "x", "y"});

  return result;
}

GameObjectTypes
Dispenser::get_types() const
{
  return {
    { "dropper", _("Dropper") },
    { "cannon", _("Cannon") },
    { "point", _("Invisible") },
    { "granito", _("Granito") }
  };
}

std::string
Dispenser::get_default_sprite_name() const
{
  switch (m_type)
  {
    case POINT:
      return "images/creatures/dispenser/invisible.sprite";
    case GRANITO:
      return "images/creatures/granito/corrupted/hive/granito_hive.sprite";
    default:
      return "images/creatures/dispenser/" + type_value_to_id(m_type) + ".sprite";
  }
}

void
Dispenser::on_flip(float height)
{
  BadGuy::on_flip(height);
  if (!m_gravity)
    FlipLevelTransformer::transform_flip(m_flip);
}


void
Dispenser::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Dispenser>("Dispenser", vm.findClass("BadGuy"));

  cls.addFunc("activate", &Dispenser::activate);
  cls.addFunc("deactivate", &Dispenser::deactivate);
}

/* EOF */
