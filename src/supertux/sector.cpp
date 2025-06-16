//  SuperTux -  A Jump'n Run
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

#include "supertux/sector.hpp"

#include <algorithm>

#include <physfs.h>
#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "collision/collision.hpp"
#include "editor/editor.hpp"
#include "math/aatriangle.hpp"
#include "math/rect.hpp"
#include "object/ambient_light.hpp"
#include "object/background.hpp"
#include "object/bullet.hpp"
#include "object/camera.hpp"
#include "object/display_effect.hpp"
#include "object/gradient.hpp"
#include "object/music_object.hpp"
#include "object/player.hpp"
#include "object/portable.hpp"
#include "object/pulsing_light.hpp"
#include "object/smoke_cloud.hpp"
#include "object/spawnpoint.hpp"
#include "object/text_array_object.hpp"
#include "object/text_object.hpp"
#include "object/tilemap.hpp"
#include "object/vertical_stripes.hpp"
#include "physfs/ifile_stream.hpp"
#include "squirrel/squirrel_environment.hpp"
#include "supertux/colorscheme.hpp"
#include "supertux/constants.hpp"
#include "supertux/debug.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/level.hpp"
#include "supertux/player_status_hud.hpp"
#include "supertux/resources.hpp"
#include "supertux/tile.hpp"
#include "supertux/tile_manager.hpp"
#include "util/file_system.hpp"
#include "util/writer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

Sector* Sector::s_current = nullptr;

Sector::Sector(Level& parent) :
  Base::Sector("sector"),
  m_level(parent),
  m_foremost_layer(),
  m_foremost_opaque_layer(),
  m_gravity(10.0f),
  m_collision_system(new CollisionSystem(*this)),
  m_text_object(add<TextObject>("Text"))
{
  add<DisplayEffect>("Effect");
  add<TextArrayObject>("TextArray");

  SoundManager::current()->preload("sounds/shoot.wav");
}

Sector::~Sector()
{
  try
  {
    deactivate();
  }
  catch(const std::exception& err)
  {
    log_warning << err.what() << std::endl;
  }

  clear_objects();
}

void
Sector::finish_construction(bool editable)
{
  flush_game_objects();
  m_initialized = false; // flush_game_objects() sets this flag to true. Sector is not yet constructed though.

  // FIXME: Is it a good idea to process some resolve requests this early?
  // I added this to fix https://github.com/SuperTux/supertux/issues/1378
  // but I don't know if it's going to introduce other bugs..   ~ Semphris
  try_process_resolve_requests();

  if (!editable)
  {
    convert_tiles2gameobject();

    if (!m_level.is_worldmap() &&
        (get_object_count<Background>() <= 0 && get_object_count<Gradient>() <= 0))
    {
      log_warning << "sector '" << get_name() << "' does not contain a background or gradient. Setting a default gradient." << std::endl;
      add<Gradient>();
    }
  }

  if (get_all_tilemaps().empty())
  {
    log_warning << "sector '" << get_name() << "' does not contain any tile layers. Creating an empty solid one." << std::endl;

    TileMap& tilemap = add<TileMap>(TileManager::current()->get_tileset(m_level.get_tileset()));
    tilemap.resize(100, 35);
    tilemap.set_solid();
  }
  else if (get_solid_tilemaps().empty())
  {
    if (editable)
    {
      log_warning << "sector '" << get_name() << "' does not contain a solid tile layer." << std::endl;
    }
    else
    {
      log_warning << "sector '" << get_name() << "' does not contain a solid tile layer. Creating an empty one." << std::endl;

      TileMap& tilemap = add<TileMap>(TileManager::current()->get_tileset(m_level.get_tileset()));
      tilemap.resize(100, 35);
      tilemap.set_solid();
    }
  }

  if (!get_object_by_type<Camera>()) {
    if (!m_level.is_worldmap())
      log_warning << "sector '" << get_name() << "' does not contain a camera." << std::endl;
    add<Camera>("Camera");
  }

  if (!get_object_by_type<AmbientLight>()) {
    add<AmbientLight>(Color::WHITE);
  }

  if (!get_object_by_type<MusicObject>()) {
    add<MusicObject>();
  }

  if (!get_object_by_type<VerticalStripes>()) {
    add<VerticalStripes>();
  }

  flush_game_objects();
  m_initialized = false; // flush_game_objects() sets this flag to true. Sector is not yet constructed though.

  m_foremost_layer = calculate_foremost_layer(false);
  m_foremost_opaque_layer = calculate_foremost_layer();

  process_resolve_requests();

  Base::Sector::finish_construction(editable);

  flush_game_objects();
}

SpawnPointMarker*
Sector::get_spawn_point(const std::string& spawnpoint)
{
  SpawnPointMarker* sp = nullptr;
  for (auto& spawn_point : get_objects_by_type<SpawnPointMarker>()) {
    if (spawn_point.get_name() == spawnpoint) {
      sp = &spawn_point;
      break;
    }
  }

  return sp;
}

Vector
Sector::get_spawn_point_position(const std::string& spawnpoint)
{
  SpawnPointMarker* sp = get_spawn_point(spawnpoint);
  if (sp)
    return sp->get_pos();
  else
    return Vector(0.0f, 0.0f);
}

void
Sector::activate(const std::string& spawnpoint)
{
  SpawnPointMarker* sp = get_spawn_point(spawnpoint);

  if (!sp) {
    if (!m_level.is_worldmap())
      log_warning << "Spawnpoint '" << spawnpoint << "' not found." << std::endl;
    if (spawnpoint != DEFAULT_SPAWNPOINT_NAME) {
      activate(DEFAULT_SPAWNPOINT_NAME);
    } else {
      activate(Vector(0, 0));
    }
  } else {
    activate(sp->get_pos());
  }
}

void
Sector::activate(const Vector& player_pos)
{
  BIND_SECTOR(*this);

  // Make sure all players are moved to this sector.
  for (auto& player : m_level.get_players())
    player->move_to_sector(*this);

  if (s_current != this) {
    if (s_current != nullptr)
      s_current->deactivate();
    s_current = this;

    m_squirrel_environment->expose_self();

    for (const auto& object : get_objects()) {
      m_squirrel_environment->expose(*object, object->get_name());
    }
  }

  // The Sector object is called 'settings' as it is accessed as 'sector.settings'
  m_squirrel_environment->expose(*this, "settings");

  if (Editor::is_active())
    return;

  // two-player hack: move other players to main player's position
  // Maybe specify 2 spawnpoints in the level?
  const auto players = get_objects_by_type<Player>();
  for (auto& player : players) {
    // spawn smalltux below spawnpoint
    if (!player.is_big()) {
      player.set_pos(player_pos + Vector(0,32));
    } else {
      player.set_pos(player_pos);
    }

    // spawning tux in the ground would kill him
    if (!is_free_of_tiles(player.get_bbox())) {
      Vector npos = player.get_bbox().p1();
      npos.y-=32;
      player.set_pos(npos);
    }
  }

  //FIXME: This is a really dirty workaround for this strange camera jump
  if (players.begin() != players.end())
  {
    Player& player = *players.begin();
    Camera& camera = get_camera();
    player.set_pos(player.get_pos()+Vector(-32, 0));
    camera.reset(player.get_pos());
    camera.update(1);
    player.set_pos(player.get_pos()+(Vector(32, 0)));
    camera.update(1);
  }

  flush_game_objects();

  //Run default.nut just before init script
  //Check to see if it's in a levelset (info file)
  std::string basedir = FileSystem::dirname(get_level().m_filename);
  if (PHYSFS_exists((basedir + "/info").c_str())) {
    try {
      IFileStream in(basedir + "/default.nut");
      m_squirrel_environment->run_script(in, "default.nut");
    } catch(std::exception& ) {
      // doesn't exist or erroneous; do nothing
    }
  }

  // Run init script
  if (!m_init_script.empty() && !Editor::is_active()) {
    run_script(m_init_script, "init-script");
  }

  // Do not interpolate camera after it has been warped
  pause_camera_interpolation();
}

void
Sector::deactivate()
{
  BIND_SECTOR(*this);

  if (s_current != this)
    return;

  m_squirrel_environment->unexpose_self();

  for (const auto& object : get_objects())
    m_squirrel_environment->unexpose(object->get_name());

  m_squirrel_environment->unexpose("settings");

  s_current = nullptr;
}

Rectf
Sector::get_active_region() const
{
  Camera& camera = get_camera();
  return Rectf(
    camera.get_translation() - Vector(1600, 1200),
    camera.get_translation() + Vector(1600, 1200) + Vector(static_cast<float>(SCREEN_WIDTH),
                                                           static_cast<float>(SCREEN_HEIGHT)));
}

int
Sector::calculate_foremost_layer(bool including_transparent) const
{
  int layer = LAYER_BACKGROUND0;
  for (auto& tm : get_objects_by_type<TileMap>())
  {
    if (tm.get_layer() > layer)
    {
      if ( including_transparent && tm.get_alpha() < 1.0f )
      {
        layer = tm.get_layer() - 1;
      }
      else
      {
        layer = tm.get_layer() + 1;
      }
    }
  }
  log_debug << "Calculated badguy falling layer was: " << layer << std::endl;
  return layer;
}

TileSet*
Sector::get_tileset() const
{
  return TileManager::current()->get_tileset(m_level.get_tileset());
}

bool
Sector::in_worldmap() const
{
  return m_level.is_worldmap();
}

void
Sector::update(float dt_sec)
{
  assert(m_initialized);

  BIND_SECTOR(*this);

  // Record last camera parameters, to allow for camera interpolation
  Camera& camera = get_camera();
  m_last_scale = camera.get_current_scale();
  m_last_translation = camera.get_translation();
  m_last_dt = dt_sec;

  m_squirrel_environment->update(dt_sec);

  GameObjectManager::update(dt_sec);

  /* Handle all possible collisions. */
  m_collision_system->update();
  flush_game_objects();
}

bool
Sector::before_object_add(GameObject& object)
{
  if (object.is_singleton())
  {
    const auto& objects = get_objects_by_type_index(std::type_index(typeid(object)));
    if (!objects.empty())
    {
      log_warning << "Can't insert multiple GameObject of type '" << typeid(object).name() << "', ignoring" << std::endl;
      return false;
    }
  }

  if (auto* movingobject = dynamic_cast<MovingObject*>(&object))
  {
    m_collision_system->add(movingobject->get_collision_object());
  }
  else if (auto* tilemap = dynamic_cast<TileMap*>(&object))
  {
    tilemap->set_ground_movement_manager(m_collision_system->get_ground_movement_manager());
  }

  if (s_current == this) {
    m_squirrel_environment->expose(object, object.get_name());
  }

  if (m_initialized) {
    try_process_resolve_requests();
    object.finish_construction();
  }

  return true;
}

void
Sector::before_object_remove(GameObject& object)
{
  auto moving_object = dynamic_cast<MovingObject*>(&object);
  if (moving_object) {
    m_collision_system->remove(moving_object->get_collision_object());
  }

  if (s_current == this)
    m_squirrel_environment->unexpose(object.get_name());
}

void
Sector::draw(DrawingContext& context)
{
  BIND_SECTOR(*this);

#if 0
  context.push_transform();

  Rect original_clip = context.get_viewport();
  context.push_transform();
  {
    Camera& camera = get_camera();
    context.set_translation(camera.get_translation());
    context.scale(camera.get_current_scale());

    get_singleton_by_type<PlayerStatusHUD>().set_target_player(0);

    Rect clip = original_clip;
    clip.left = (clip.left + clip.right) / 2 + 16;
    context.set_viewport(clip);

    GameObjectManager::draw(context);

    if (g_debug.show_collision_rects) {
      m_collision_system->draw(context);
    }
  }
  context.set_viewport(original_clip);
  context.pop_transform();

  context.push_transform();
  {
    Camera& camera = get_camera();
    context.set_translation(camera.get_translation());
    context.scale(camera.get_current_scale());

    get_singleton_by_type<PlayerStatusHUD>().set_target_player(1);

    Rect clip = original_clip;
    clip.right = (clip.left + clip.right) / 2 - 16;
    context.set_viewport(clip);

    GameObjectManager::draw(context);

    if (g_debug.show_collision_rects) {
      m_collision_system->draw(context);
    }
  }
  context.set_viewport(original_clip);
  context.pop_transform();

  context.pop_transform();

  Rect midline = original_clip;
  midline.right = (original_clip.left + original_clip.right) / 2 - 16;
  midline.left = (original_clip.left + original_clip.right) / 2 + 16;
  context.color().draw_filled_rect(midline, Color::BLACK, 99999);
#else
  context.push_transform();

  Camera& camera = get_camera();

  if (g_config->frame_prediction && m_last_dt > 0.f) {
    // Interpolate between two camera settings; there are many possible ways to do this, but on
    // short time scales all look about the same. This delays the camera position by one frame.
    // (The proper thing to do, of course, would be not to interpolate, but instead to adjust
    // the Camera class to extrapolate, and provide scale/translation at a given time; done
    // right, this would make it possible to, for example, exactly sinusoidally shake the
    // camera instead of piecewise linearly.)
    float x = std::min(1.f, context.get_time_offset() / m_last_dt);
    context.set_translation(camera.get_translation() * x + (1 - x) * m_last_translation);
    context.scale(camera.get_current_scale() * x + (1 - x) * m_last_scale);
  } else {
    context.set_translation(camera.get_translation());
    context.scale(camera.get_current_scale());
  }

  GameObjectManager::draw(context);

  if (g_debug.show_collision_rects) {
    m_collision_system->draw(context);
  }

  context.pop_transform();
#endif

  if (m_level.m_is_in_cutscene && !m_level.m_skip_cutscene)
  {
    context.color().draw_text(Resources::normal_font,
                              _("Press escape to skip"),
                              Vector(32.f, 32.f),
                              ALIGN_LEFT,
                              LAYER_OBJECTS + 1000,
                              ColorScheme::Text::heading_color);
  }
}

bool
Sector::is_free_of_tiles(const Rectf& rect, const bool ignoreUnisolid, uint32_t tiletype) const
{
  return m_collision_system->is_free_of_tiles(rect, ignoreUnisolid, tiletype);
}

bool
Sector::is_free_of_solid_tiles(float left, float top, float right, float bottom,
                               bool ignore_unisolid) const
{
  return m_collision_system->is_free_of_tiles(Rectf(Vector(left, top), Vector(right, bottom)),
                                              ignore_unisolid, Tile::SOLID);
}

bool
Sector::is_free_of_statics(const Rectf& rect, const MovingObject* ignore_object, const bool ignoreUnisolid, uint32_t tiletype) const
{
  return m_collision_system->is_free_of_statics(rect,
                                                ignore_object ? ignore_object->get_collision_object() : nullptr,
                                                ignoreUnisolid,
                                                tiletype);
}

bool
Sector::is_free_of_statics(float left, float top, float right, float bottom,
                           bool ignore_unisolid) const
{
  return m_collision_system->is_free_of_statics(Rectf(Vector(left, top), Vector(right, bottom)),
                                                nullptr, ignore_unisolid);
}

bool
Sector::is_free_of_movingstatics(const Rectf& rect, const MovingObject* ignore_object, bool ignore_unisolid) const
{
  return m_collision_system->is_free_of_movingstatics(rect,
                                                      ignore_object ? ignore_object->get_collision_object() : nullptr,
                                                      ignore_unisolid);
}

bool
Sector::is_free_of_movingstatics(float left, float top, float right, float bottom) const
{
  return m_collision_system->is_free_of_movingstatics(Rectf(Vector(left, top), Vector(right, bottom)), nullptr);
}

bool
Sector::is_free_of_specifically_movingstatics(const Rectf& rect, const MovingObject* ignore_object) const
{
  return m_collision_system->is_free_of_specifically_movingstatics(rect,
                                                      ignore_object ? ignore_object->get_collision_object() : nullptr);
}

bool
Sector::is_free_of_specifically_movingstatics(float left, float top, float right, float bottom) const
{
  return m_collision_system->is_free_of_specifically_movingstatics(Rectf(Vector(left, top), Vector(right, bottom)), nullptr);
}

CollisionSystem::RaycastResult
Sector::get_first_line_intersection(const Vector& line_start,
                                    const Vector& line_end,
                                    CollisionSystem::RaycastIgnore ignore,
                                    const CollisionObject* ignore_object) const {
  return m_collision_system->get_first_line_intersection(line_start, line_end, ignore, ignore_object);
}

CollisionSystem::RaycastResult
Sector::get_first_line_intersection(const Vector& line_start,
                                    const Vector& line_end,
                                    bool ignore_objects,
                                    const CollisionObject* ignore_object) const {
  auto ignore = (ignore_objects ? CollisionSystem::IGNORE_OBJECTS : CollisionSystem::IGNORE_NONE);
  return m_collision_system->get_first_line_intersection(line_start, line_end, ignore, ignore_object);
}

bool
Sector::free_line_of_sight(const Vector& line_start, const Vector& line_end, bool ignore_objects, const MovingObject* ignore_object) const
{
  return m_collision_system->free_line_of_sight(line_start, line_end, ignore_objects,
                                                ignore_object ? ignore_object->get_collision_object() : nullptr);
}

bool
Sector::can_see_player(const Vector& eye) const
{
  for (auto& player : get_objects_by_type<Player>())
  {
    // test for free line of sight to any of all four corners and the middle of the player's bounding box
    if (free_line_of_sight(eye, player.get_bbox().p1(), false, &player)) return true;
    if (free_line_of_sight(eye, Vector(player.get_bbox().get_right(), player.get_bbox().get_top()), false, &player)) return true;
    if (free_line_of_sight(eye, player.get_bbox().p2(), false, &player)) return true;
    if (free_line_of_sight(eye, Vector(player.get_bbox().get_left(), player.get_bbox().get_bottom()), false, &player)) return true;
    if (free_line_of_sight(eye, player.get_bbox().get_middle(), false, &player)) return true;
  }
  return false;
}

bool
Sector::inside(const Rectf& rect) const
{
  for (const auto& tilemap : get_all_tilemaps()) {
    Rectf bbox = tilemap->get_bbox();

    // the top of the sector extends to infinity
    if (bbox.get_left() <= rect.get_left() &&
        rect.get_right() <= bbox.get_right() &&
        rect.get_bottom() <= bbox.get_bottom()) {
      return true;
    }
  }
  return false;
}

Size
Sector::get_editor_size() const
{
  // Find the tilemap with the greatest surface
  size_t max_surface = 0;
  Size size;
  for (const auto& tilemap : get_all_tilemaps()) {
    size_t surface = tilemap->get_width() * tilemap->get_height();
    if (surface > max_surface) {
      max_surface = surface;
      size = tilemap->get_size();
    }
  }

  return size;
}

void
Sector::resize(const Size& old_size, const Size& new_size, const Size& resize_offset)
{
  BIND_SECTOR(*this);

  const bool is_offset = resize_offset.width || resize_offset.height;
  const Vector obj_shift(static_cast<float>(resize_offset.width) * 32.0f,
                         static_cast<float>(resize_offset.height) * 32.0f);

  for (auto* tilemap : get_all_tilemaps())
  {
    if (tilemap->get_size() == old_size)
    {
      tilemap->save_state();
      tilemap->resize(new_size, resize_offset);
      tilemap->check_state();
    }
    else if (is_offset)
    {
      tilemap->save_state();
      tilemap->move_by(obj_shift);
      tilemap->check_state();
    }
  }

  if (is_offset)
  {
    for (auto& object : get_objects_by_type<MovingObject>())
    {
      object.save_state();
      object.move_to(object.get_pos() + obj_shift);
      object.check_state();
    }
  }
}

void
Sector::change_solid_tiles(uint32_t old_tile_id, uint32_t new_tile_id)
{
  for (auto& solids: get_solid_tilemaps()) {
    solids->change_all(old_tile_id, new_tile_id);
  }
}

Player*
Sector::get_nearest_player(const Vector& pos) const
{
  auto players = get_objects_by_type_index(typeid(Player));
  if (players.size() == 1)
  {
    Player* player = static_cast<Player*>(players[0]);
    return (!player->is_alive() ? nullptr : player);
  }

  Player* nearest_player = nullptr;
  float nearest_dist = std::numeric_limits<float>::max();

  for (auto player_ptr : players)
  {
    Player* player = static_cast<Player*>(player_ptr);
    if (!player->is_alive())
      continue;

    float dist = player->get_bbox().distance(pos);

    if (dist < nearest_dist)
    {
      nearest_player = player;
      nearest_dist = dist;
    }
  }

  return nearest_player;
}

std::vector<MovingObject*>
Sector::get_nearby_objects(const Vector& center, float max_distance) const
{
  std::vector<MovingObject*> result;
  for (auto& object : m_collision_system->get_nearby_objects(center, max_distance))
  {
    result.push_back(&object->get_parent());
  }
  return result;
}

void
Sector::stop_looping_sounds()
{
  for (auto& object : get_objects()) {
    object->stop_looping_sounds();
  }
}

void
Sector::pause_camera_interpolation()
{
  m_last_dt = 0.;
}

void Sector::play_looping_sounds()
{
  for (const auto& object : get_objects()) {
    object->play_looping_sounds();
  }
}

void
Sector::save(Writer &writer)
{
  BIND_SECTOR(*this);

  writer.start_list("sector", false);

  writer.write("name", m_name, false);

  if (!m_level.is_worldmap()) {
    if (m_gravity != 10.0f) {
      writer.write("gravity", m_gravity);
    }
  }

  if (m_init_script.size()) {
    writer.write("init-script", m_init_script,false);
  }

  // saving objects;
  std::vector<GameObject*> objects(get_objects().size());
  std::transform(get_objects().begin(), get_objects().end(), objects.begin(), [] (auto& obj) {
    return obj.get();
  });

  std::stable_sort(objects.begin(), objects.end(),
                   [](const GameObject* lhs, GameObject* rhs) {
                     return lhs->get_class_name() < rhs->get_class_name();
                   });

  for (auto& obj : objects) {
    if (obj->is_saveable()) {
      writer.start_list(obj->get_class_name());
      obj->save(writer);
      writer.end_list(obj->get_class_name());
    }
  }

  writer.end_list("sector");
}

void
Sector::convert_tiles2gameobject()
{
  // add lights for special tiles
  for (auto& tm : get_objects_by_type<TileMap>())
  {
    // Since object setup is not yet complete, I have to manually add the offset
    // See https://github.com/SuperTux/supertux/issues/1378 for details
    Vector tm_offset = tm.get_path() ? tm.get_path()->get_base() : Vector(0, 0);

    for (int x=0; x < tm.get_width(); ++x)
    {
      for (int y=0; y < tm.get_height(); ++y)
      {
        const Tile& tile = tm.get_tile(x, y);

        if (!tile.get_object_name().empty())
        {
          // If a tile is associated with an object, insert that
          // object and remove the tile
          if (tile.get_object_name() == "decal" ||
              tm.is_solid())
          {
            Vector pos = tm.get_tile_position(x, y) + tm_offset;
            try {
              auto object = GameObjectFactory::instance().create(tile.get_object_name(), pos, Direction::AUTO, tile.get_object_data());
              add_object(std::move(object));
              tm.change(x, y, 0);
            } catch(std::exception& e) {
              log_warning << e.what() << "" << std::endl;
            }
          }
        }
        else
        {
          // add lights for fire tiles
          uint32_t attributes = tile.get_attributes();
          Vector pos = tm.get_tile_position(x, y);
          Vector center = pos + Vector(16, 16);

          if (attributes & Tile::FIRE) {
            if (attributes & Tile::HURTS) {
              // lava or lavaflow
              // space lights a bit
              if ((tm.get_tile(x-1, y).get_attributes() != attributes || x%3 == 0)
                  && (tm.get_tile(x, y-1).get_attributes() != attributes || y%3 == 0)) {
                float pseudo_rnd = static_cast<float>(static_cast<int>(pos.x) % 10) / 10;
                add<PulsingLight>(center, 1.0f + pseudo_rnd, 0.8f, 1.0f,
                                  Color(1.0f, 0.3f, 0.0f, 1.0f), &tm);
              }
            } else {
              // torch
              float pseudo_rnd = static_cast<float>(static_cast<int>(pos.x) % 10) / 10;
              add<PulsingLight>(center, 1.0f + pseudo_rnd, 0.9f, 1.0f,
                                Color(1.0f, 1.0f, 0.6f, 1.0f), &tm);
            }
          }
        }
      }
    }
  }
}

Camera&
Sector::get_camera() const
{
  return get_singleton_by_type<Camera>();
}

DisplayEffect&
Sector::get_effect() const
{
  return get_singleton_by_type<DisplayEffect>();
}

std::vector<Player*>
Sector::get_players() const
{
  return m_level.get_players();
}


void
Sector::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Sector>("Sector", vm.findClass("GameObjectManager"));

  cls.addFunc("set_gravity", &Sector::set_gravity);
  cls.addFunc("get_gravity", &Sector::get_gravity);
  cls.addFunc("is_free_of_solid_tiles", &Sector::is_free_of_solid_tiles);
  cls.addFunc<bool, Sector, float, float, float, float, bool>("is_free_of_statics", &Sector::is_free_of_statics);
  cls.addFunc<bool, Sector, float, float, float, float>("is_free_of_movingstatics", &Sector::is_free_of_movingstatics);
  cls.addFunc<bool, Sector, float, float, float, float>("is_free_of_specifically_movingstatics", &Sector::is_free_of_specifically_movingstatics);

  cls.addVar("gravity", &Sector::m_gravity);
}

/* EOF */
