//  SuperTux -  A Jump'n Run
//  Copyright (C) 2004 Ingo Ruhnke <grumbel@gmail.com>
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

#include "worldmap/tux.hpp"

#include <sstream>

#include "control/input_manager.hpp"
#include "editor/editor.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/savegame.hpp"
#include "supertux/tile.hpp"
#include "util/log.hpp"
#include "worldmap/camera.hpp"
#include "worldmap/direction.hpp"
#include "worldmap/level_tile.hpp"
#include "worldmap/special_tile.hpp"
#include "worldmap/sprite_change.hpp"
#include "worldmap/teleporter.hpp"
#include "worldmap/worldmap.hpp"

namespace worldmap {

static const float TUXSPEED = 200;
static const float map_message_TIME = 2.8f;

Tux::Tux(WorldMap* worldmap) :
  m_back_direction(),
  m_worldmap(worldmap),
  m_sprite(SpriteManager::current()->create(m_worldmap->get_savegame().get_player_status().worldmap_sprite)),
  m_controller(InputManager::current()->get_controller()),
  m_input_direction(Direction::NONE),
  m_direction(Direction::NONE),
  m_initial_tile_pos(),
  m_tile_pos(),
  m_offset(0),
  m_moving(false),
  m_ghost_mode(false)
{
}

void
Tux::draw(DrawingContext& context)
{
  if (m_worldmap->get_sector().get_camera().is_panning()) return;

  std::string action = get_action_prefix_for_bonus(m_worldmap->get_savegame().get_player_status().bonus[0]);
  if (!action.empty())
  {
    if (m_moving && (get_axis().x != 0 || get_axis().y != 0))
    {
      std::string direct = "-up";
      if (get_axis().x == 1) direct = "-right";
      if (get_axis().x == -1) direct = "-left"; 
      if (get_axis().y == 1) direct = "-up"; 
      if (get_axis().y == -1) direct = "-down"; 
      if (m_sprite->has_action(action + "-walking" + direct))
      {
        m_sprite->set_action(action + "-walking" + direct);
      }
      else if (m_sprite->has_action(action + "-walking"))
      {
        m_sprite->set_action(action + "-walking");
      }
      // else, keep the same animation that was already playing
    }
    else
    {
      m_sprite->set_action(action + "-stop");
    }
  }
  else
  {
    log_debug << "Bonus type not handled in worldmap." << std::endl;
    m_sprite->set_action("large-stop");
  }
  m_sprite->draw(context.color(), get_pos(), LAYER_OBJECTS + 1);
}

std::string
Tux::get_action_prefix_for_bonus(const BonusType& bonus) const
{
  if (bonus == BONUS_GROWUP)
    return "large";
  if (bonus == BONUS_FIRE)
    return "fire";
  if (bonus == BONUS_ICE)
    return "ice";
  if (bonus == BONUS_AIR)
    return "air";
  if (bonus == BONUS_EARTH)
    return "earth";
  if (bonus == BONUS_NONE)
    return "small";

  return "";
}

Vector
Tux::get_pos() const
{
  float x = m_tile_pos.x * 32;
  float y = m_tile_pos.y * 32;

  switch (m_direction)
  {
    case Direction::WEST:
      x -= m_offset - 32;
      break;
    case Direction::EAST:
      x += m_offset - 32;
      break;
    case Direction::NORTH:
      y -= m_offset - 32;
      break;
    case Direction::SOUTH:
      y += m_offset - 32;
      break;
    case Direction::NONE:
      break;
  }

  return Vector(x, y);
}

Vector
Tux::get_axis() const
{
  float x = 0.0f;
  float y = 0.0f;

  switch (m_direction)
  {
    case Direction::WEST:
      x = -1.0f;
      break;
    case Direction::EAST:
      x = 1.0f;
      break;
    case Direction::NORTH:
      y = 1.0f;
      break;
    case Direction::SOUTH:
      y = -1.0f;
      break;
    case Direction::NONE:
      break;
  }

  return Vector(x, y);
}

void
Tux::stop()
{
  m_offset = 0;
  m_direction = Direction::NONE;
  m_input_direction = Direction::NONE;
  m_moving = false;
}

void
Tux::try_start_walking()
{
  if (m_moving)
    return;
  if (m_input_direction == Direction::NONE)
    return;

  auto level = m_worldmap->get_sector().at_object<LevelTile>();

  // We got a new direction, so lets start walking when possible
  Vector next_tile(0.0f, 0.0f);
  if ((!level || level->is_solved() || level->is_perfect()
      || (Editor::current() && Editor::current()->is_testing_level()))
      && m_worldmap->get_sector().path_ok(m_input_direction, m_tile_pos, &next_tile)) {
    m_tile_pos = next_tile;
    m_moving = true;
    m_direction = m_input_direction;
    m_back_direction = reverse_dir(m_direction);
  } else if (m_ghost_mode || (m_input_direction == m_back_direction)) {
    m_moving = true;
    m_direction = m_input_direction;
    m_tile_pos = m_worldmap->get_sector().get_next_tile(m_tile_pos, m_direction);
    m_back_direction = reverse_dir(m_direction);
  }
}

bool
Tux::can_walk(int tile_data, Direction dir) const
{
  return m_ghost_mode ||
    ((tile_data & Tile::WORLDMAP_NORTH && dir == Direction::NORTH) ||
     (tile_data & Tile::WORLDMAP_SOUTH && dir == Direction::SOUTH) ||
     (tile_data & Tile::WORLDMAP_EAST  && dir == Direction::EAST) ||
     (tile_data & Tile::WORLDMAP_WEST  && dir == Direction::WEST));
}

void
Tux::change_sprite(SpriteChange* sprite_change)
{
  //SpriteChange* sprite_change = m_worldmap->at_object<SpriteChange>();
  if (sprite_change != nullptr) {
    m_sprite = sprite_change->clone_sprite();
    sprite_change->clear_stay_action();
    m_worldmap->get_savegame().get_player_status().worldmap_sprite = sprite_change->get_sprite_name();
  }
}

void
Tux::try_continue_walking(float dt_sec)
{
  if (!m_moving)
    return;

  // Let tux walk
  m_offset += TUXSPEED * dt_sec;

  // Do nothing if we have not yet reached the next tile
  if (m_offset <= 32)
    return;

  m_offset -= 32;

  auto worldmap_sector = &m_worldmap->get_sector();

  auto sprite_change = worldmap_sector->at_object<SpriteChange>(m_tile_pos);
  change_sprite(sprite_change);

  // if this is a special_tile with passive_message, display it
  auto special_tile = worldmap_sector->at_object<SpecialTile>();
  if (special_tile)
  {
    // direction and the apply_action_ are opposites, since they "see"
    // directions in a different way
    if ((m_direction == Direction::NORTH && special_tile->get_apply_action_south()) ||
        (m_direction == Direction::SOUTH && special_tile->get_apply_action_north()) ||
        (m_direction == Direction::WEST && special_tile->get_apply_action_east()) ||
        (m_direction == Direction::EAST && special_tile->get_apply_action_west()))
    {
      process_special_tile(special_tile);
    }
  }

  // check if we are at a Teleporter
  auto teleporter = worldmap_sector->at_object<Teleporter>(m_tile_pos);

  // stop if we reached a level, a WORLDMAP_STOP tile, a teleporter or a special tile without a passive_message
  if ((worldmap_sector->at_object<LevelTile>()) ||
      (worldmap_sector->tile_data_at(m_tile_pos) & Tile::WORLDMAP_STOP) ||
      (special_tile && !special_tile->is_passive_message() && special_tile->get_script().empty()) ||
      (teleporter) ||
      m_ghost_mode)
  {
    if (special_tile && !special_tile->get_map_message().empty() && !special_tile->is_passive_message()) {
      m_worldmap->set_passive_message({}, 0.0f);
    }
    stop();
    return;
  }

  // if user wants to change direction, try changing, else guess the direction in which to walk next
  const int tile_data = worldmap_sector->tile_data_at(m_tile_pos);
  if ((m_direction != m_input_direction) && can_walk(tile_data, m_input_direction)) {
    m_direction = m_input_direction;
    m_back_direction = reverse_dir(m_direction);
  } else {
    Direction dir = Direction::NONE;
    if (tile_data & Tile::WORLDMAP_NORTH && m_back_direction != Direction::NORTH)
      dir = Direction::NORTH;
    else if (tile_data & Tile::WORLDMAP_SOUTH && m_back_direction != Direction::SOUTH)
      dir = Direction::SOUTH;
    else if (tile_data & Tile::WORLDMAP_EAST && m_back_direction != Direction::EAST)
      dir = Direction::EAST;
    else if (tile_data & Tile::WORLDMAP_WEST && m_back_direction != Direction::WEST)
      dir = Direction::WEST;

    if (dir == Direction::NONE) {
      // Should never be reached if tiledata is good
      log_warning << "Could not determine where to walk next" << std::endl;
      stop();
      return;
    }

    m_direction = dir;
    m_input_direction = m_direction;
    m_back_direction = reverse_dir(m_direction);
  }

  // Walk automatically to the next tile
  if (m_direction == Direction::NONE)
    return;

  Vector next_tile(0.0f, 0.0f);
  if (!m_ghost_mode && !worldmap_sector->path_ok(m_direction, m_tile_pos, &next_tile)) {
    log_debug << "Tilemap data is buggy" << std::endl;
    stop();
    return;
  }

  auto next_sprite = worldmap_sector->at_object<SpriteChange>(next_tile);
  if (next_sprite != nullptr && next_sprite->change_on_touch()) {
    change_sprite(next_sprite);
  }
  //SpriteChange* last_sprite = m_worldmap->at_object<SpriteChange>(next_tile);
  if (sprite_change != nullptr && next_sprite != nullptr) {
    log_debug << "Old: " << m_tile_pos << " New: " << next_tile << std::endl;
    sprite_change->set_stay_action();
  }

  m_tile_pos = next_tile;
}

void
Tux::update_input_direction()
{
  if (m_controller.hold(Control::UP))
    m_input_direction = Direction::NORTH;
  else if (m_controller.hold(Control::DOWN))
    m_input_direction = Direction::SOUTH;
  else if (m_controller.hold(Control::LEFT))
    m_input_direction = Direction::WEST;
  else if (m_controller.hold(Control::RIGHT))
    m_input_direction = Direction::EAST;
}

void
Tux::update(float dt_sec)
{
  if (m_worldmap->get_sector().get_camera().is_panning()) return;

  update_input_direction();
  if (m_moving)
    try_continue_walking(dt_sec);
  else
    try_start_walking();
}

void
Tux::setup()
{
  // Set initial tile position, if provided
  if (m_initial_tile_pos != Vector())
    m_tile_pos = m_initial_tile_pos;

  // check if we already touch a SpriteChange object
  auto sprite_change = m_worldmap->get_sector().at_object<SpriteChange>(m_tile_pos);
  change_sprite(sprite_change);
}

void
Tux::process_special_tile(SpecialTile* special_tile)
{
  if (!special_tile) {
    return;
  }

  if (special_tile->is_passive_message())
    m_worldmap->set_passive_message(special_tile->get_map_message(), map_message_TIME);

  if (!special_tile->get_script().empty())
  {
    try
    {
      m_worldmap->get_sector().run_script(special_tile->get_script(), "specialtile");
    }
    catch(std::exception& e)
    {
      log_warning << "Couldn't execute special tile script: " << e.what()
                  << std::endl;
    }
  }
}

} // namespace worldmap

/* EOF */
