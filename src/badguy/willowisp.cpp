//  SuperTux - "Will-O-Wisp" Badguy
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

#include "badguy/willowisp.hpp"

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "badguy/dispenser.hpp"
#include "editor/editor.hpp"
#include "object/lantern.hpp"
#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "supertux/constants.hpp"
#include "supertux/game_session.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

static const float FLYSPEED = 64.0f; /**< Speed in pixels per second. */
static const float TRACK_RANGE = 384.0f; /**< Distance at which to start tracking the player. */
static const float VANISH_RANGE = 512.0f; /**< Distance at which to stop tracking and vanish. */
static const std::string SOUNDFILE = "sounds/willowisp.wav";

WillOWisp::WillOWisp(const ReaderMapping& reader) :
  BadGuy(reader, "images/creatures/willowisp/willowisp.sprite", LAYER_FLOATINGOBJECTS,
         "images/objects/lightmap_light/lightmap_light-small.sprite"),
  PathObject(),
  m_mystate(STATE_IDLE),
  m_target_sector(),
  m_target_spawnpoint(),
  m_hit_script(),
  m_sound_source(),
  m_flyspeed(),
  m_track_range(),
  m_vanish_range(),
  m_color(0, 1, 0),
  m_starting_node(0)
{
  if (Editor::is_active())
  {
    reader.get("sector", m_target_sector);
    reader.get("spawnpoint", m_target_spawnpoint);
  }
  else
  {
    reader.get("sector", m_target_sector, DEFAULT_SECTOR_NAME.c_str());
    reader.get("spawnpoint", m_target_spawnpoint, DEFAULT_SPAWNPOINT_NAME.c_str());
  }

  reader.get("flyspeed", m_flyspeed, FLYSPEED);
  reader.get("track-range", m_track_range, TRACK_RANGE);
  reader.get("vanish-range", m_vanish_range, VANISH_RANGE);
  reader.get("hit-script", m_hit_script, "");

  bool running = false;
  reader.get("running", running);

  std::vector<float> color;
  if (reader.get("color", color))
  {
    m_color = Color(color);
  }

  reader.get("starting-node", m_starting_node, 0);

  init_path(reader, running);

  m_countMe = false;
  SoundManager::current()->preload(SOUNDFILE);
  SoundManager::current()->preload("sounds/warp.wav");

  m_lightsprite->set_color(Color(m_color.red * 0.2f,
                                 m_color.green * 0.2f,
                                 m_color.blue * 0.2f));
  m_sprite->set_color(m_color);
  m_glowing = true;

  set_action("idle");
}

void
WillOWisp::synchronize_position_from_path()
{
  if (!has_valid_path())
    return;

  const auto& nodes = get_path()->get_nodes();
  if (nodes.empty())
    return;

  const int size = static_cast<int>(nodes.size());

  if (m_starting_node < 0)
  {
    m_starting_node = 0;
  }
  if (m_starting_node >= size)
  {
    m_starting_node = size - 1;
  }

  set_pos(m_path_handle.get_pos(m_col.m_bbox.get_size(), nodes[m_starting_node].position));
}

void
WillOWisp::finish_construction()
{
  if (!get_path())
    init_path_pos(m_col.m_bbox.p1());

  synchronize_position_from_path();

  if (has_valid_path())
  {
    const auto& nodes = get_path()->get_nodes();
    if (!nodes.empty())
    {
      get_walker()->jump_to_node(m_starting_node);

      if (get_walker()->is_running())
      {
        m_mystate = STATE_PATHMOVING_TRACK;
        get_walker()->start_moving();
      }
    }
  }
}

void
WillOWisp::editor_update()
{
  synchronize_position_from_path();
}

void
WillOWisp::after_editor_set()
{
  BadGuy::after_editor_set();

  m_lightsprite->set_color(Color(m_color.red * 0.2f,
                                 m_color.green * 0.2f,
                                 m_color.blue * 0.2f));
  m_sprite->set_color(m_color);

  if (Editor::is_active())
  {
    synchronize_position_from_path();
  }
}

void
WillOWisp::active_update(float dt_sec)
{
  if (Editor::is_active() && get_path() && get_path()->is_valid())
  {
    get_walker()->update(dt_sec);
    set_pos(get_walker()->get_pos(m_col.m_bbox.get_size(), m_path_handle));
    return;
  }

  auto player = get_nearest_player();
  if (!player) return;
  const Vector p1 = m_col.m_bbox.get_middle();
  const Vector p2 = player->get_bbox().get_middle();
  Vector dist = (p2 - p1);

  switch (m_mystate)
  {
    case STATE_STOPPED:
      break;
    case STATE_IDLE:
      if (glm::length(dist) <= m_track_range)
      {
        m_mystate = STATE_TRACKING;
      }
      break;
    case STATE_TRACKING:
      if (glm::length(dist) > m_vanish_range)
      {
        vanish();
      }
      else if (glm::length(dist) >= 1)
      {
        Vector dir_ = glm::normalize(dist);
        m_col.set_movement(dir_ * dt_sec * m_flyspeed);
      }
      else
      {
        /* We somehow landed right on top of the player without colliding.
         * Sit tight and avoid a division by zero. */
      }
      m_sound_source->set_position(get_pos());
      break;
    case STATE_WARPING:
      m_col.set_movement(Vector(0.0f, 0.0f));
      if (m_sprite->animation_done())
      {
        remove_me();
      }
      break;
    case STATE_VANISHING:
    {
      Vector dir_ = glm::normalize(dist);
      m_col.set_movement(dir_ * dt_sec * m_flyspeed);
      if (m_sprite->animation_done())
      {
        remove_me();
      }
      break;
    }

    case STATE_PATHMOVING:
    case STATE_PATHMOVING_TRACK:
      if (!PathObject::has_valid_path())
      {
        m_mystate = STATE_IDLE;
        return;
      }

      get_walker()->update(dt_sec);

      m_col.set_movement(get_walker()->get_pos(m_col.m_bbox.get_size(), m_path_handle) - get_pos());
      if (m_mystate == STATE_PATHMOVING_TRACK && glm::length(dist) <= m_track_range)
      {
        m_mystate = STATE_TRACKING;
      }

      break;
    default:
      assert(false);
  }
}

void
WillOWisp::activate()
{
  if (Editor::is_active())
    return;

  m_sound_source = SoundManager::current()->create_sound_source(SOUNDFILE);
  m_sound_source->set_position(get_pos());
  m_sound_source->set_looping(true);
  m_sound_source->set_gain(1.0f);
  m_sound_source->set_reference_distance(32);
  m_sound_source->play();
}

void
WillOWisp::deactivate()
{
  m_sound_source.reset(nullptr);

  switch (m_mystate)
  {
    case STATE_STOPPED:
    case STATE_IDLE:
    case STATE_PATHMOVING:
    case STATE_PATHMOVING_TRACK:
      break;
    case STATE_TRACKING:
      m_mystate = STATE_IDLE;
      break;
    case STATE_WARPING:
    case STATE_VANISHING:
      remove_me();
      break;
  }
}

void
WillOWisp::vanish()
{
  m_mystate = STATE_VANISHING;
  set_action("vanishing", 1);
  set_colgroup_active(COLGROUP_DISABLED);

  if (m_parent_dispenser != nullptr)
  {
    m_parent_dispenser->notify_dead();
  }
}

bool
WillOWisp::collides(MovingObject& other, const CollisionHit&) const
{
  auto lantern = dynamic_cast<Lantern*>(&other);

  //                                 vv  'xor'
  if (lantern && (lantern->is_open() != (get_color().greyscale() == 0)))
    return true;

  if (dynamic_cast<Player*>(&other))
    return true;

  return false;
}

HitResponse
WillOWisp::collision_player(Player& player, const CollisionHit&)
{
  if (player.is_invincible())
    return ABORT_MOVE;

  if (m_mystate != STATE_TRACKING)
    return ABORT_MOVE;

  m_mystate = STATE_WARPING;
  set_action("warping", 1);

  if (!m_hit_script.empty())
  {
    Sector::get().run_script(m_hit_script, "hit-script");
  }
  else
  {
    GameSession::current()->respawn(m_target_sector, m_target_spawnpoint);
  }
  SoundManager::current()->play("sounds/warp.wav", get_pos());

  return CONTINUE;
}

void
WillOWisp::goto_node(int node_idx)
{
  // If the wisp is following a player refuse to "snap back" to the path it was supposed to follow.
  if (m_mystate == STATE_TRACKING)
    return;

  if (!has_valid_path())
  {
    return;
  }

  PathObject::goto_node(node_idx);

  if (m_mystate != STATE_PATHMOVING && m_mystate != STATE_PATHMOVING_TRACK)
    m_mystate = STATE_PATHMOVING;
}

void
WillOWisp::set_state(const std::string& new_state)
{
  if (new_state == "stopped")
    m_mystate = STATE_STOPPED;
  else if (new_state == "idle")
    m_mystate = STATE_IDLE;
  else if (new_state == "move_path")
  {
    if (has_valid_path())
    {
      m_mystate = STATE_PATHMOVING;
      if (get_walker())
        get_walker()->start_moving();
    }
    else
    {
      m_mystate = STATE_IDLE;
    }
  }
  else if (new_state == "move_path_track")
  {
    if (has_valid_path())
    {
      m_mystate = STATE_PATHMOVING_TRACK;
      if (get_walker()) get_walker()->start_moving();
    }
    else
    {
      m_mystate = STATE_IDLE;
    }
  }
  else if (new_state == "normal")
    m_mystate = STATE_IDLE;
  else if (new_state == "vanish")
    vanish();
  else
    log_warning << "Cannot set an unknown WillOWisp state: '" << new_state << "'." << std::endl;
}

ObjectSettings
WillOWisp::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_text(_("Sector"), &m_target_sector, "sector");
  result.add_text(_("Spawnpoint"), &m_target_spawnpoint, "spawnpoint");
  result.add_text(_("Hit script"), &m_hit_script, "hit-script");
  result.add_float(_("Track range"), &m_track_range, "track-range", TRACK_RANGE);
  result.add_float(_("Vanish range"), &m_vanish_range, "vanish-range", VANISH_RANGE);
  result.add_float(_("Fly speed"), &m_flyspeed, "flyspeed", FLYSPEED);
  result.add_path_ref(_("Path"), *this, get_path_ref(), "path-ref");
  result.add_int(_("Starting Node"), &m_starting_node, "starting-node", 0, 0U);
  result.add_color(_("Color"), &m_color, "color");

  if (get_path_gameobject() && get_path())
  {
    result.add_walk_mode(_("Path Mode"), &get_path()->m_mode, {}, {});
    result.add_bool(_("Adapt Speed"), &get_path()->m_adapt_speed, "adapt-speed", {});
    result.add_path_handle(_("Handle"), m_path_handle, "handle");
    result.add_bool(_("Running"), &get_walker()->m_running, "running", false, 0);
  }

  result.reorder({ "name", "x", "y", "width", "height", "path-ref", "starting-node", "running", "path-walk-mode", "adapt-speed", "handle", "flyspeed", "track-range", "vanish-range", "sector", "spawnpoint", "hit-script", "color" });

  return result;
}

void
WillOWisp::stop_looping_sounds()
{
  if (m_sound_source)
  {
    m_sound_source->stop();
  }
}

void
WillOWisp::play_looping_sounds()
{
  if (m_sound_source)
  {
    m_sound_source->play();
  }
}

void
WillOWisp::move_to(const Vector& pos)
{
  Vector shift = pos - m_col.m_bbox.p1();
  if (get_path())
  {
    get_path()->move_by(shift);
  }
  set_pos(pos);
}

std::vector<Direction>
WillOWisp::get_allowed_directions() const
{
  return {};
}

void
WillOWisp::on_flip(float height)
{
  BadGuy::on_flip(height);
  PathObject::on_flip();
}


void
WillOWisp::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<WillOWisp>("WillOWisp", vm.findClass("BadGuy"));

  PathObject::register_members(cls);

  cls.addFunc("goto_node", &WillOWisp::goto_node);
  cls.addFunc("set_state", &WillOWisp::set_state);
}

/* EOF */
