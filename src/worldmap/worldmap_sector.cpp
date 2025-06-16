//  SuperTux -  A Jump'n Run
//  Copyright (C) 2004 Ingo Ruhnke <grumbel@gmail.com>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//                2023 Vankata453
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

#include "worldmap/worldmap_sector.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "audio/sound_manager.hpp"
#include "object/ambient_light.hpp"
#include "object/display_effect.hpp"
#include "object/music_object.hpp"
#include "object/tilemap.hpp"
#include "physfs/ifile_stream.hpp"
#include "squirrel/squirrel_environment.hpp"
#include "supertux/constants.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/debug.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/game_session.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/level.hpp"
#include "supertux/player_status_hud.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/shrinkfade.hpp"
#include "supertux/tile.hpp"
#include "util/file_system.hpp"
#include "worldmap/camera.hpp"
#include "worldmap/level_tile.hpp"
#include "worldmap/spawn_point.hpp"
#include "worldmap/special_tile.hpp"
#include "worldmap/teleporter.hpp"
#include "worldmap/worldmap.hpp"

namespace worldmap {

WorldMapSector*
WorldMapSector::current()
{
  if (!WorldMap::current())
    return nullptr;

  return &WorldMap::current()->get_sector();
}


WorldMapSector::WorldMapSector(WorldMap& parent) :
  Base::Sector("worldmap"),
  m_parent(parent),
  m_camera(new Camera(*this)),
  m_tux(&add<Tux>(&parent)),
  m_spawnpoints(),
  m_initial_fade_tilemap(),
  m_fade_direction()
{
  BIND_WORLDMAP_SECTOR(*this);

  add<PlayerStatusHUD>(m_parent.get_savegame().get_player_status());
}

WorldMapSector::~WorldMapSector()
{
  m_spawnpoints.clear();

  clear_objects();
}

void
WorldMapSector::finish_construction(bool editable)
{
  flush_game_objects();

  if (!get_object_by_type<AmbientLight>())
    add<AmbientLight>(Color::WHITE);

  if (!get_object_by_type<MusicObject>())
    add<MusicObject>();

  if (!get_object_by_type<DisplayEffect>())
    add<DisplayEffect>("Effect");

  flush_game_objects();

  Base::Sector::finish_construction(editable);
}


void
WorldMapSector::setup()
{
  BIND_WORLDMAP_SECTOR(*this);

  ScreenManager::current()->set_screen_fade(std::make_unique<FadeToBlack>(FadeToBlack::FADEIN, 1.0f));

  // If we specified a fade tilemap, let's fade it:
  if (!m_initial_fade_tilemap.empty())
  {
    auto tilemap = get_object_by_name<TileMap>(m_initial_fade_tilemap);
    if (tilemap != nullptr)
    {
      if (m_fade_direction == 0)
      {
        tilemap->fade(1.0, 1);
      }
      else
      {
        tilemap->fade(0.0, 1);
      }
    }
    m_initial_fade_tilemap = "";
  }

  m_tux->setup();

  // register worldmap_table as "worldmap" in scripting
  m_squirrel_environment->expose_self();
  m_squirrel_environment->expose(*this, "settings");

  /** Perform scripting related actions. **/
  // Run default.nut just before init script
  try
  {
    IFileStream in(m_parent.get_levels_path() + "default.nut");
    m_squirrel_environment->run_script(in, "WorldMapSector::default.nut");
  }
  catch (...)
  {
    // doesn't exist or erroneous; do nothing
  }

  if (!m_init_script.empty())
    m_squirrel_environment->run_script(m_init_script, "WorldMapSector::init");

  // Check if Tux is on an auto-playing level.
  // No need to play music in that case.
  LevelTile* level = at_object<LevelTile>();
  if(level && level->is_auto_play() && !level->is_solved())
    return;

  auto& music_object = get_singleton_by_type<MusicObject>();
  music_object.play_music(MusicType::LEVEL_MUSIC);
}

void
WorldMapSector::leave()
{
  BIND_WORLDMAP_SECTOR(*this);

  // remove worldmap_table from roottable
  m_squirrel_environment->unexpose_self();
}


void
WorldMapSector::draw(DrawingContext& context)
{
  BIND_WORLDMAP_SECTOR(*this);

  if (get_width() < context.get_width() ||
      get_height() < context.get_height())
  {
    context.color().draw_filled_rect(context.get_rect(),
                                     Color(0.0f, 0.0f, 0.0f, 1.0f), LAYER_BACKGROUND0);
  }

  context.push_transform();
  context.set_translation(m_camera->get_offset());

  GameObjectManager::draw(context);

  if (g_debug.show_worldmap_path)
  {
    for (int x = 0; x < static_cast<int>(get_tiles_width()); x++) {
      for (int y = 0; y < static_cast<int>(get_tiles_height()); y++) {
        const int data = tile_data_at(Vector(static_cast<float>(x), static_cast<float>(y)));
        const int px = x * 32;
        const int py = y * 32;
        const int W = 4;
        const int layer = LAYER_FOREGROUND1 + 1000;
        const Color color(1.0f, 0.0f, 1.0f, 0.5f);
        if (data & Tile::WORLDMAP_NORTH)    context.color().draw_filled_rect(Rect(px + 16-W, py       , px + 16+W, py + 16-W), color, layer);
        if (data & Tile::WORLDMAP_SOUTH)    context.color().draw_filled_rect(Rect(px + 16-W, py + 16+W, px + 16+W, py + 32  ), color, layer);
        if (data & Tile::WORLDMAP_EAST)     context.color().draw_filled_rect(Rect(px + 16+W, py + 16-W, px + 32  , py + 16+W), color, layer);
        if (data & Tile::WORLDMAP_WEST)     context.color().draw_filled_rect(Rect(px       , py + 16-W, px + 16-W, py + 16+W), color, layer);
        if (data & Tile::WORLDMAP_DIR_MASK) context.color().draw_filled_rect(Rect(px + 16-W, py + 16-W, px + 16+W, py + 16+W), color, layer);
        if (data & Tile::WORLDMAP_STOP)     context.color().draw_filled_rect(Rect(px + 4   , py + 4   , px + 28  , py + 28  ), color, layer);
      }
    }
  }

  draw_status(context);
}

void
WorldMapSector::draw_status(DrawingContext& context)
{
  context.push_transform();
  context.set_translation(Vector(0, 0));

  if (!m_tux->is_moving()) {
    LevelTile* level = at_object<LevelTile>();
    if (level)
    {
      context.color().draw_text(Resources::normal_font, level->get_title(),
                                Vector(context.get_width() / 2.0f,
                                       context.get_height() - Resources::normal_font->get_height() - 10),
                                ALIGN_CENTER, LAYER_HUD, level->get_title_color());

      if (g_config->developer_mode) {
        context.color().draw_text(Resources::small_font, FileSystem::join(level->get_basedir(), level->get_level_filename()),
                                  Vector(context.get_width() / 2.0f,
                                         context.get_height() - Resources::normal_font->get_height() - 25),
                                  ALIGN_CENTER, LAYER_HUD, level->get_title_color());
      }

      // if level is solved, draw level picture behind stats
      /*
        if (level.solved) {
        if (const Surface* picture = level->get_picture()) {
        Vector pos = Vector(context.get_width() - picture->get_width(), context.get_height() - picture->get_height());
        context.push_transform();
        context.set_alpha(0.5);
        context.color().draw_surface(picture, pos, LAYER_FOREGROUND1-1);
        context.pop_transform();
        }
        }
      */
      level->get_statistics().draw_worldmap_info(context, level->get_target_time());
    }

    SpecialTile* special_tile = at_object<SpecialTile>();
    if (special_tile)
    {
      /* Display an in-map message in the map, if any as been selected */
      if (!special_tile->get_map_message().empty() && !special_tile->is_passive_message())
        context.color().draw_text(Resources::normal_font, special_tile->get_map_message(),
                                  Vector(context.get_width() / 2.0f,
                                         context.get_height() - static_cast<float>(Resources::normal_font->get_height()) - 60.0f),
                                  ALIGN_CENTER, LAYER_FOREGROUND1, WorldMap::s_message_color);
    }

    // display teleporter messages
    Teleporter* teleporter = at_object<Teleporter>();
    if (teleporter && (!teleporter->get_message().empty()))
    {
      Vector pos = Vector(context.get_width() / 2.0f,
                          context.get_height() - Resources::normal_font->get_height() - 30.0f);
      context.color().draw_text(Resources::normal_font, teleporter->get_message(), pos, ALIGN_CENTER, LAYER_FOREGROUND1, WorldMap::s_teleporter_message_color);
    }
  }

  /* Display a passive message on the map, if set */
  if (m_parent.m_passive_message_timer.started())
    context.color().draw_text(Resources::normal_font, m_parent.m_passive_message,
                              Vector(context.get_width() / 2.0f,
                                     context.get_height() - Resources::normal_font->get_height() - 60.0f),
                              ALIGN_CENTER, LAYER_FOREGROUND1, WorldMap::s_message_color);

  context.pop_transform();
}

void
WorldMapSector::update(float dt_sec)
{
  BIND_WORLDMAP_SECTOR(*this);

  GameObjectManager::update(dt_sec);

  m_camera->update(dt_sec);

  {
    if(!m_tux->is_moving())
    {
      // check for teleporters
      auto teleporter = at_object<Teleporter>();
      if (teleporter && (teleporter->is_automatic() || (m_parent.m_enter_level))) {
        m_parent.m_enter_level = false;
        if (!teleporter->get_worldmap().empty())
        {
          // Change worldmap.
          m_parent.change(teleporter->get_worldmap(), teleporter->get_sector(),
                          teleporter->get_spawnpoint());
        }
        else
        {
          // TODO: an animation, camera scrolling or a fading would be a nice touch
          SoundManager::current()->play("sounds/warp.wav");
          m_tux->m_back_direction = Direction::NONE;
          if (!teleporter->get_sector().empty())
          {
            // A target sector is set, so teleport to it at the specified spawnpoint.
            m_parent.set_sector(teleporter->get_sector(), teleporter->get_spawnpoint());
          }
          else
          {
            // No target sector is set, so teleport at the specified spawnpoint in the current one.
            move_to_spawnpoint(teleporter->get_spawnpoint(), true);
          }
        }
      }
    }
  }

  {
    if(!m_tux->is_moving())
    {
      // check for auto-play levels
      auto level = at_object<LevelTile>();
      if (level && level->is_auto_play() && !level->is_solved()) {
        m_parent.m_enter_level = true;
        // automatically mark these levels as solved in case player aborts
        level->set_solved(true);
      }
    }
  }

  {
    if (m_parent.m_enter_level && !m_tux->is_moving())
    {
      /* Check level action */
      auto level_ = at_object<LevelTile>();
      auto tux_pos = m_tux->get_tile_pos();
      if (!level_) {
        // Respawn if player on a tile with no level and nowhere to go.
        if (!is_valid_path_at(tux_pos)) {
          log_warning << "Player at illegal position " << tux_pos.x << ", " << tux_pos.y << " respawning." << std::endl;
          move_to_spawnpoint(DEFAULT_SPAWNPOINT_NAME);
          return;
        }
        log_warning << "No level to enter at: " << tux_pos.x << ", " << tux_pos.y << std::endl;
        return;
      }

      if (level_->get_tile_pos() == tux_pos) {
        try {
          Vector shrinkpos = Vector(level_->get_pos().x + 16 - m_camera->get_offset().x,
                                    level_->get_pos().y +  8 - m_camera->get_offset().y);
          std::string levelfile = m_parent.m_levels_path + level_->get_level_filename();
          
          auto game_session = std::make_unique<GameSession>(levelfile, m_parent.m_savegame, &level_->get_statistics());
          game_session->restart_level();

          // update state and savegame
          m_parent.save_state();
          ScreenManager::current()->push_screen(std::move(game_session),
                                                std::make_unique<ShrinkFade>(shrinkpos, 1.0f, LAYER_LIGHTMAP - 1));

          m_parent.m_in_level = true;
        } catch(std::exception& e) {
          log_fatal << "Couldn't load level: " << e.what() << std::endl;
        }
      }
    }
    else
    {
      // tux->set_direction(input_direction);
    }
  }

  flush_game_objects();
}


MovingObject&
WorldMapSector::add_object_scripting(const std::string& class_name, const std::string& name,
                                     const Vector& pos, const std::string& direction,
                                     const std::string& data)
{
  if (!GameObjectFactory::instance().has_params(class_name, ObjectFactory::OBJ_PARAM_WORLDMAP))
    throw std::runtime_error("Object '" + class_name + "' cannot be added to a worldmap sector.");

  auto& obj = GameObjectManager::add_object_scripting(class_name, name, pos, direction, data);

  // Set position of non-WorldMapObjects from provided tile position.
  if (!dynamic_cast<WorldMapObject*>(&obj))
    obj.set_pos(obj.get_pos() * 32.f);

  return obj;
}


Vector
WorldMapSector::get_next_tile(const Vector& pos, const Direction& direction) const
{
  auto position = pos;
  switch (direction) {
    case Direction::WEST:
      position.x -= 1;
      break;
    case Direction::EAST:
      position.x += 1;
      break;
    case Direction::NORTH:
      position.y -= 1;
      break;
    case Direction::SOUTH:
      position.y += 1;
      break;
    case Direction::NONE:
      break;
  }
  return position;
}

int
WorldMapSector::available_directions_at(const Vector& p) const
{
  return tile_data_at(p) & Tile::WORLDMAP_DIR_MASK;
}

int
WorldMapSector::tile_data_at(const Vector& p) const
{
  int dirs = 0;

  for (const auto& tilemap : get_solid_tilemaps()) {
    const Tile& tile = tilemap->get_tile(static_cast<int>(p.x), static_cast<int>(p.y));
    int dirdata = tile.get_data();
    dirs |= dirdata;
  }

  return dirs;
}

bool
WorldMapSector::is_valid_path_at(const Vector& p) const
{
  const auto tile_data = tile_data_at(p);
  return tile_data & (Tile::WORLDMAP_NORTH | Tile::WORLDMAP_SOUTH |
                      Tile::WORLDMAP_WEST  | Tile::WORLDMAP_EAST); 
}

size_t
WorldMapSector::level_count() const
{
  return get_object_count<LevelTile>();
}

size_t
WorldMapSector::solved_level_count() const
{
  size_t count = 0;
  for (auto& level : get_objects_by_type<LevelTile>()) {
    if (level.is_solved()) {
      count++;
    }
  }

  return count;
}


void
WorldMapSector::finished_level(Level* gamelevel)
{
  // TODO use Level* parameter here?
  auto level = at_object<LevelTile>();

  if (level == nullptr) {
    return;
  }

  bool old_level_state = level->is_solved();
  level->set_solved(true);

  // deal with statistics
  level->get_statistics().update(gamelevel->m_stats);

  if (level->get_statistics().completed(level->get_target_time())) {
    level->set_perfect(true);
  }

  m_parent.save_state();

  if (old_level_state != level->is_solved()) {
    // Try to detect the next direction to which we should walk
    // FIXME: Mostly a hack
    Direction dir = Direction::NONE;

    int dirdata = available_directions_at(m_tux->get_tile_pos());
    // first, test for crossroads
    if (dirdata == Tile::WORLDMAP_CNSE ||
        dirdata == Tile::WORLDMAP_CNSW ||
        dirdata == Tile::WORLDMAP_CNEW ||
        dirdata == Tile::WORLDMAP_CSEW ||
        dirdata == Tile::WORLDMAP_CNSEW)
      dir = Direction::NONE;
    else if (dirdata & Tile::WORLDMAP_NORTH
             && m_tux->m_back_direction != Direction::NORTH)
      dir = Direction::NORTH;
    else if (dirdata & Tile::WORLDMAP_SOUTH
             && m_tux->m_back_direction != Direction::SOUTH)
      dir = Direction::SOUTH;
    else if (dirdata & Tile::WORLDMAP_EAST
             && m_tux->m_back_direction != Direction::EAST)
      dir = Direction::EAST;
    else if (dirdata & Tile::WORLDMAP_WEST
             && m_tux->m_back_direction != Direction::WEST)
      dir = Direction::WEST;

    if (dir != Direction::NONE) {
      m_tux->set_direction(dir);
    }
  }

  if (!level->get_extro_script().empty()) {
    try {
      run_script(level->get_extro_script(), "WorldMapSector:extro_script");
    } catch(std::exception& e) {
      log_warning << "Couldn't run level-extro-script: " << e.what() << std::endl;
    }
  }
}

SpawnPoint*
WorldMapSector::get_spawnpoint_by_name(const std::string& spawnpoint_name) const
{
  auto spawnpoint = std::find_if(m_spawnpoints.begin(), m_spawnpoints.end(), 
    [spawnpoint_name](const auto& sp) {
      return sp->get_name() == spawnpoint_name;
    });
  return spawnpoint != m_spawnpoints.end() ? spawnpoint->get() : nullptr;
}

bool
WorldMapSector::path_ok(const Direction& direction, const Vector& old_pos, Vector* new_pos) const
{
  *new_pos = get_next_tile(old_pos, direction);
 
  if (!(new_pos->x >= 0 && new_pos->x < get_tiles_width()
        && new_pos->y >= 0 && new_pos->y < get_tiles_height()))
  { // New position is outsite the tilemap
    return false;
  }
  else
  { // Check if the tile allows us to go to new_pos
    int old_tile_data = tile_data_at(old_pos);
    int new_tile_data = tile_data_at(*new_pos);
    switch (direction)
    {
      case Direction::WEST:
        return (old_tile_data & Tile::WORLDMAP_WEST
                && new_tile_data & Tile::WORLDMAP_EAST);

      case Direction::EAST:
        return (old_tile_data & Tile::WORLDMAP_EAST
                && new_tile_data & Tile::WORLDMAP_WEST);

      case Direction::NORTH:
        return (old_tile_data & Tile::WORLDMAP_NORTH
                && new_tile_data & Tile::WORLDMAP_SOUTH);

      case Direction::SOUTH:
        return (old_tile_data & Tile::WORLDMAP_SOUTH
                && new_tile_data & Tile::WORLDMAP_NORTH);

      case Direction::NONE:
        log_warning << "path_ok() can't walk if direction is NONE" << std::endl;
        assert(false);
    }
    return false;
  }
}

void
WorldMapSector::set_sector(const std::string& sector)
{
  m_parent.set_sector(sector);
}

void
WorldMapSector::spawn(const std::string& sector, const std::string& spawnpoint)
{
  m_parent.set_sector(sector, spawnpoint);
}

void
WorldMapSector::move_to_spawnpoint(const std::string& spawnpoint)
{
  move_to_spawnpoint(spawnpoint, false);
}

void
WorldMapSector::move_to_spawnpoint(const std::string& spawnpoint, bool pan)
{
  auto sp = get_spawnpoint_by_name(spawnpoint);
  if (sp != nullptr) {
    Vector p = sp->get_pos();
    m_tux->set_tile_pos(p);
    m_tux->set_direction(sp->get_auto_dir());
    if (pan) {
      m_camera->pan();
    }
    return;
  }

  log_warning << "Spawnpoint '" << spawnpoint << "' not found." << std::endl;
  if (spawnpoint != DEFAULT_SPAWNPOINT_NAME) {
    move_to_spawnpoint(DEFAULT_SPAWNPOINT_NAME);
  }
}

void
WorldMapSector::set_initial_fade_tilemap(const std::string& tilemap_name, int direction)
{
  m_initial_fade_tilemap = tilemap_name;
  m_fade_direction = direction;
}


TileSet*
WorldMapSector::get_tileset() const
{
  return m_parent.m_tileset;
}

Vector
WorldMapSector::get_tux_pos() const
{
  return m_tux->get_pos();
}

float
WorldMapSector::get_tux_x() const
{
  return m_tux->get_pos().x;
}

float
WorldMapSector::get_tux_y() const
{
  return m_tux->get_pos().y;
}

std::string
WorldMapSector::get_filename() const
{
  return m_parent.get_filename();
}

void
WorldMapSector::set_title_level(const std::string& filename)
{
  m_parent.get_savegame().get_player_status().title_level = filename;
}


void
WorldMapSector::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<WorldMapSector>("WorldMapSector", vm.findClass("GameObjectManager"));

  cls.addFunc("get_tux_x", &WorldMapSector::get_tux_x);
  cls.addFunc("get_tux_y", &WorldMapSector::get_tux_y);
  cls.addFunc("set_sector", &WorldMapSector::set_sector);
  cls.addFunc("spawn", &WorldMapSector::spawn);
  cls.addFunc<void, WorldMapSector, const std::string&>("move_to_spawnpoint", &WorldMapSector::move_to_spawnpoint);
  cls.addFunc("get_filename", &WorldMapSector::get_filename);
  cls.addFunc("set_title_level", &WorldMapSector::set_title_level);
}

} // namespace worldmap

/* EOF */
