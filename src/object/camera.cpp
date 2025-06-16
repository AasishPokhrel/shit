//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "object/camera.hpp"

#include <math.h>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "math/random.hpp"
#include "math/util.hpp"
#include "object/player.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/level.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

/* Very small value, used in Camera checks. */
static const float CAMERA_EPSILON = .00001f;

/**
 * For the multiplayer camera, the camera will ensure all players are visible.
 * These variables allow establishing a minimum zone around them that will also
 * be always visible.
 */
static const float HORIZONTAL_MARGIN = 196.f; // 6 tiles.
static const float VERTICAL_MARGIN = 196.f; // 6 tiles.
static const float PEEK_ADD_HORIZONTAL_MARGIN = 320.f; // 10 tiles.
static const float PEEK_ADD_VERTICAL_MARGIN = 320.f; // 10 tiles.

/* 0 = no movement, 1 = no smooth adaptation. */
static const float MULTIPLAYER_CAM_WEIGHT = 0.1f;

Camera::Camera(const std::string& name) :
  LayerObject(name),
  m_mode(Mode::NORMAL),
  m_defaultmode(Mode::NORMAL),
  m_screen_size(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)),
  m_translation(0.0f, 0.0f),
  m_lookahead_pos(0.0f, 0.0f),
  m_peek_pos(0.0f, 0.0f),
  m_cached_translation(0.0f, 0.0f),
  m_shaketimer(),
  m_shakespeed(),
  m_shakedepth_x(),
  m_shakedepth_y(),
  m_earthquake(false),
  m_earthquake_strength(),
  m_earthquake_delay(),
  m_earthquake_last_offset(0.f),
  m_earthquake_delay_timer(),
  m_scroll_from(0.0f, 0.0f),
  m_scroll_goal(0.0f, 0.0f),
  m_scroll_to_pos(),
  m_scrollspeed(),
  m_scale(1.f),
  m_scale_origin(1.f),
  m_scale_target(1.f),
  m_scale_time_total(0.f),
  m_scale_time_remaining(0.f),
  m_scale_origin_translation(),
  m_scale_target_translation(),
  m_scale_easing(),
  m_scale_anchor(),
  m_minimum_scale(1.f),
  m_enfore_minimum_scale(false)
{
}

Camera::Camera(const ReaderMapping& reader) :
  LayerObject(reader),
  m_mode(Mode::NORMAL),
  m_defaultmode(Mode::NORMAL),
  m_screen_size(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)),
  m_translation(0.0f, 0.0f),
  m_lookahead_pos(0.0f, 0.0f),
  m_peek_pos(0.0f, 0.0f),
  m_cached_translation(0.0f, 0.0f),
  m_shaketimer(),
  m_shakespeed(),
  m_shakedepth_x(),
  m_shakedepth_y(),
  m_earthquake(false),
  m_earthquake_strength(),
  m_earthquake_delay(),
  m_earthquake_last_offset(0.f),
  m_earthquake_delay_timer(),
  m_scroll_from(0.0f, 0.0f),
  m_scroll_goal(0.0f, 0.0f),
  m_scroll_to_pos(),
  m_scrollspeed(),
  m_scale(1.f),
  m_scale_origin(1.f),
  m_scale_target(1.f),
  m_scale_time_total(0.f),
  m_scale_time_remaining(0.f),
  m_scale_origin_translation(),
  m_scale_target_translation(),
  m_scale_easing(),
  m_scale_anchor(),
  m_minimum_scale(1.f),
  m_enfore_minimum_scale(false)
{
  std::string modename;

  reader.get("mode", modename);
  if (modename == "normal")
  {
    m_mode = Mode::NORMAL;
  }
  else if (modename == "autoscroll")
  {
    m_mode = Mode::AUTOSCROLL;

    init_path(reader, true);
  }
  else if (modename == "manual")
  {
    m_mode = Mode::MANUAL;
  }
  else
  {
    m_mode = Mode::NORMAL;
    log_warning << "invalid camera mode '" << modename << "'found in worldfile." << std::endl;
  }
  m_defaultmode = m_mode;

  if (m_name.empty()) {
    m_name = "Camera";
  }
}

Camera::~Camera()
{
}

ObjectSettings
Camera::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_enum(_("Mode"), reinterpret_cast<int*>(&m_defaultmode),
                  {_("normal"), _("manual"), _("autoscroll")},
                  {"normal", "manual", "autoscroll"},
                  {}, "mode");

  result.add_path_ref(_("Path"), *this, get_path_ref(), "path-ref");

  if (has_valid_path()) {
    result.add_walk_mode(_("Path Mode"), &get_path()->m_mode, {}, {});
    result.add_bool(_("Adapt Speed"), &get_path()->m_adapt_speed, {}, {});
    result.add_path_handle(_("Handle"), m_path_handle, "handle");
  }

  return result;
}

void
Camera::after_editor_set()
{
  if (has_valid_path()) {
    if (m_defaultmode != Mode::AUTOSCROLL) {
      get_path()->m_nodes.clear();
      auto path_obj = get_path_gameobject();
      if(path_obj != nullptr)
      {
        path_obj->editor_delete();
      }
    }
  } else {
    if (m_defaultmode == Mode::AUTOSCROLL) {
      init_path_pos(Vector(0,0));
    }
  }
}

void
Camera::save_state()
{
  GameObject::save_state();
  PathObject::save_state();
}

void
Camera::check_state()
{
  GameObject::check_state();
  PathObject::check_state();
}

const Vector
Camera::get_translation() const
{
  Vector screen_size = m_screen_size.as_vector();
  return m_translation + ((screen_size * (get_current_scale() - 1.f)) / 2.f);
}

void
Camera::set_translation_centered(const Vector& translation)
{
  m_translation = translation - m_screen_size.as_vector() / 2;
}

Rectf
Camera::get_rect() const
{
  return Rectf::from_center(get_center(), m_screen_size);
}

void
Camera::reset(const Vector& tuxpos)
{
  m_translation.x = tuxpos.x - m_screen_size.width / 2.0f;
  m_translation.y = tuxpos.y - m_screen_size.height / 2.0f;

  m_shakespeed = 0;
  m_shaketimer.stop();
  keep_in_bounds(m_translation);

  m_cached_translation = m_translation;
}

void
Camera::shake(float duration, float x, float y)
{
  m_shaketimer.start(duration*100.f);
  m_shakedepth_x = x;
  m_shakedepth_y = y;
  m_shakespeed = math::PI_2 / duration;
}

void
Camera::start_earthquake(float strength, float delay)
{
  if (strength <= 0.f)
  {
    log_warning << "Invalid earthquake strength value provided. Setting to 3." << std::endl;
    strength = 3.f;
  }
  if (delay <= 0.f)
  {
    log_warning << "Invalid earthquake delay value provided. Setting to 0.05." << std::endl;
    delay = 0.05f;
  }

  m_earthquake = true;
  m_earthquake_strength = strength;
  m_earthquake_delay = delay;
}

void
Camera::stop_earthquake()
{
  m_translation.y -= m_earthquake_last_offset;
  m_cached_translation.y -= m_earthquake_last_offset;

  m_earthquake = false;
  m_earthquake_last_offset = 0.f;
  m_earthquake_delay_timer.stop();
}

void
Camera::scroll_to(const Vector& goal, float scrolltime)
{
  if(scrolltime == 0.0f)
  {
    m_translation.x = goal.x;
    m_translation.y = goal.y;
    m_mode = Mode::MANUAL;
    return;
  }

  m_scroll_from = m_translation;
  m_scroll_goal = goal;
  keep_in_bounds(m_scroll_goal);

  m_scroll_to_pos = 0;
  m_scrollspeed = 1.f / scrolltime;
  m_mode = Mode::SCROLLTO;
}

void
Camera::update(float dt_sec)
{
  // Fetch the current screen size from the VideoSystem. The main loop always
  // processes input and window events, updates game logic, and then draws zero
  // or more frames, in that order, so this screen size will be used by the next
  // draw() operation.
  m_screen_size = Sizef(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)) / get_current_scale();

  // Minimum scale should be set during the update sequence; else, reset it.
  m_enfore_minimum_scale = false;

  switch (m_mode) {
    case Mode::NORMAL:
      if (Sector::current() && Sector::current()->get_object_count<Player>() > 1)
      {
        update_scroll_normal_multiplayer(dt_sec);
      }
      else
      {
        update_scroll_normal(dt_sec);
      }
      break;
    case Mode::MANUAL:
      keep_in_bounds(m_translation);
      break;
    case Mode::AUTOSCROLL:
      update_scroll_autoscroll(dt_sec);
      break;
    case Mode::SCROLLTO:
      update_scroll_to(dt_sec);
      break;
    default:
      break;
  }

  update_scale(dt_sec);
  update_shake();
  update_earthquake();
}

void
Camera::keep_in_bounds(const Rectf& bounds)
{
  Sizef screen_size = Sizef(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)) / get_current_scale();

  // Determines the difference between normal and scaled translation.
  const Vector scale_factor = (screen_size.as_vector() * (get_current_scale() - 1.f)) / 2.f;

  // Keep the translation's scaled position in provided bounds.
  m_translation.x = (bounds.get_width() > screen_size.width ?
      math::clamp(m_translation.x + scale_factor.x, bounds.get_left(), bounds.get_right() - screen_size.width) :
      bounds.get_left());
  m_translation.y = (bounds.get_height() > screen_size.height ?
      math::clamp(m_translation.y + scale_factor.y, bounds.get_top(), bounds.get_bottom() - screen_size.height) :
      bounds.get_top());

  // Remove any scale factor we may have added in the checks above.
  m_translation -= scale_factor;
}

void
Camera::keep_in_bounds(Vector& translation_)
{
  float width = get_parent()->get_width();
  float height = get_parent()->get_height();

  // Remove any earthquake offset from the translation.
  translation_.y -= m_earthquake_last_offset;

  if (m_mode == Mode::MANUAL)
  {
    // Determines the difference between normal and scaled translation.
    const Vector scale_factor = (m_screen_size.as_vector() * (get_current_scale() - 1.f)) / 2.f;

    // Keep the translation's scaled position in sector bounds.
    translation_.x = math::clamp(translation_.x + scale_factor.x, 0.0f, width - m_screen_size.width);
    translation_.y = math::clamp(translation_.y + scale_factor.y, 0.0f, height - m_screen_size.height);

    // Remove any scale factor we may have added in the checks above.
    translation_ -= scale_factor;
  }
  else
  {
    // Don't scroll before the start or after the sector's end.
    translation_.x = math::clamp(translation_.x, 0.0f, width - m_screen_size.width);
    translation_.y = math::clamp(translation_.y, 0.0f, height - m_screen_size.height);
  }

  // Add any earthquake offset we may have removed earlier.
  translation_.y += m_earthquake_last_offset;

  if (height < m_screen_size.height)
    translation_.y = height / 2.0f - m_screen_size.height / 2.0f;
  if (width < m_screen_size.width)
    translation_.x = width / 2.0f - m_screen_size.width / 2.0f;
}

void
Camera::update_shake()
{
  if (m_shaketimer.started()) {

    // Old method:

    // m_translation.x -= sinf(m_shaketimer.get_timegone() * m_shakespeed) * m_shakedepth_x;
    // m_translation.y -= sinf(m_shaketimer.get_timegone() * m_shakespeed) * m_shakedepth_y;

    // Elastic easing:

    m_translation.x -= m_shakedepth_x * ((std::pow(2.f, -0.8f * (m_shakespeed * m_shaketimer.get_timegone()))) *
      std::sin(((0.8f * m_shakespeed * m_shaketimer.get_timegone()) - 0.75f) * (2.f * math::PI) / 3.f));
    m_translation.y -= m_shakedepth_y * ((std::pow(2.f, -0.8f * (m_shakespeed * m_shaketimer.get_timegone()))) *
      std::sin(((0.8f * m_shakespeed * m_shaketimer.get_timegone()) - 0.75f) * (2.f * math::PI) / 3.f));
  }
}

void
Camera::update_earthquake()
{
  if (!m_earthquake)
    return;

  if (m_earthquake_delay_timer.check())
  {
    if (m_earthquake_last_offset == 0.f)
    {
      m_earthquake_last_offset = m_earthquake_strength * graphicsRandom.randf(-2, 2);
      m_translation.y += m_earthquake_last_offset;
      m_cached_translation.y += m_earthquake_last_offset;
    }
    else
    {
      m_translation.y -= m_earthquake_last_offset;
      m_cached_translation.y -= m_earthquake_last_offset;
      m_earthquake_last_offset = 0.f;
    }

    m_earthquake_delay_timer.start(m_earthquake_delay + static_cast<float>(graphicsRandom.rand(0, 1)));
  }
  else if (!m_earthquake_delay_timer.started())
  {
    m_earthquake_delay_timer.start(m_earthquake_delay + static_cast<float>(graphicsRandom.rand(0, 1)));
  }
}

void
Camera::update_scroll_normal(float dt_sec)
{
  Player& player = *d_sector->get_players()[0];
  // TODO: Co-op mode needs a good camera.
  const Vector player_pos(player.get_bbox().get_left(),
                          player.get_bbox().get_bottom());
  static Vector last_player_pos = player_pos;
  const Vector player_delta = player_pos - last_player_pos;
  last_player_pos = player_pos;

  // Check that we don't have division by zero later.
  if (dt_sec < CAMERA_EPSILON)
    return;

  /****** Vertical Scrolling part. ******/
  if (!player.is_dying())
  {
    m_cached_translation.y = math::clamp(m_cached_translation.y,
                                         player_pos.y - m_screen_size.height * 0.67f,
                                         player_pos.y - m_screen_size.height * 0.33f);
  }

  m_translation.y = m_cached_translation.y;

  if (!player.is_dying())
  {
    const float top_edge = m_screen_size.height * 0.3f;
    const float bottom_edge = m_screen_size.height * 0.7f;
    const float translation_compensation_y = player_pos.y - m_translation.y;

    float peek_to_y = 0;
    if (player.peeking_direction_y() == Direction::UP)
      peek_to_y = bottom_edge - translation_compensation_y;
    else if (player.peeking_direction_y() == Direction::DOWN)
      peek_to_y = top_edge - translation_compensation_y;

    if (m_translation.y + m_screen_size.height < get_parent()->get_height())
      m_peek_pos.y += (peek_to_y - m_peek_pos.y) * g_config->camera_peek_multiplier;

    m_translation.y -= m_peek_pos.y;
    m_translation.y = math::clamp(m_translation.y,
                                  player_pos.y - m_screen_size.height * 0.7f,
                                  player_pos.y - m_screen_size.height * 0.3f);
    m_cached_translation.y = math::clamp(m_cached_translation.y,
                                         player_pos.y - m_screen_size.height * 0.7f,
                                         player_pos.y - m_screen_size.height * 0.3f);
  }

  /****** Horizontal scrolling part *******/
  if (!player.is_dying())
  {
    const float left_end = m_screen_size.width * 0.4f;
    const float right_end = m_screen_size.width * 0.6f;

    if (player_delta.x < -CAMERA_EPSILON || player_delta.x > CAMERA_EPSILON) // Walking left or right.
      m_lookahead_pos.x -= player_delta.x * 0.8f;

    m_lookahead_pos.x = math::clamp(m_lookahead_pos.x, left_end, right_end);

    // Adjust for level ends.
    if (player_pos.x < left_end)
      m_lookahead_pos.x = left_end;
    if (player_pos.x > d_sector->get_width() - left_end)
      m_lookahead_pos.x = right_end;

    m_cached_translation.x = player_pos.x - m_lookahead_pos.x;
  }

  m_translation.x = m_cached_translation.x;

  if (!player.is_dying())
  {
    const float left_edge = m_screen_size.width * 0.1666f;
    const float right_edge = m_screen_size.width * 0.8334f;
    const float translation_compensation_x = player_pos.x - m_translation.x;

    float peek_to_x = 0;
    if (player.peeking_direction_x() == Direction::LEFT)
      peek_to_x = right_edge - translation_compensation_x;
    else if (player.peeking_direction_x() == Direction::RIGHT)
      peek_to_x = left_edge - translation_compensation_x;

    m_peek_pos.x += (peek_to_x - m_peek_pos.x) * g_config->camera_peek_multiplier;

    m_translation.x -= m_peek_pos.x;
    m_translation.x = math::clamp(m_translation.x,
                                  player_pos.x - m_screen_size.width * 0.8334f,
                                  player_pos.x - m_screen_size.width * 0.1666f);
    m_cached_translation.x = math::clamp(m_cached_translation.x,
                                         player_pos.x - m_screen_size.width * 0.8334f,
                                         player_pos.x - m_screen_size.width * 0.1666f);
  }

  keep_in_bounds(m_translation);
  keep_in_bounds(m_cached_translation);
}

void
Camera::update_scroll_normal_multiplayer(float dt_sec)
{
  m_enfore_minimum_scale = true;

  float x1 = Sector::get().get_width();
  float y1 = Sector::get().get_height();
  float x2 = 0.f;
  float y2 = 0.f;

  for (const auto* p : Sector::get().get_players())
  {
    if (!p->is_alive())
      continue;

    float lft = p->get_bbox().get_left() - HORIZONTAL_MARGIN;
    float rgt = p->get_bbox().get_right() + HORIZONTAL_MARGIN;
    float top = p->get_bbox().get_top() - VERTICAL_MARGIN;
    float btm = p->get_bbox().get_bottom() + VERTICAL_MARGIN;

    if (p->peeking_direction_x() == Direction::LEFT)
      lft -= PEEK_ADD_HORIZONTAL_MARGIN;
    else if (p->peeking_direction_x() == Direction::RIGHT)
      rgt += PEEK_ADD_HORIZONTAL_MARGIN;

    if (p->peeking_direction_y() == Direction::UP)
      top -= PEEK_ADD_VERTICAL_MARGIN;
    else if (p->peeking_direction_y() == Direction::DOWN)
      btm += PEEK_ADD_VERTICAL_MARGIN;

    x1 = std::min(x1, lft);
    x2 = std::max(x2, rgt);
    y1 = std::min(y1, top);
    y2 = std::max(y2, btm);
  }

  // Might happen if all players are dead.
  if (x2 < x1 || y2 < y1)
    return;

  Rectf cover(std::max(0.f, x1),
              std::max(0.f, y1),
              std::min(Sector::get().get_width(), x2),
              std::min(Sector::get().get_height(), y2));

  float scale = std::min(static_cast<float>(SCREEN_WIDTH) / static_cast<float>(cover.get_width()),
                         static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(cover.get_height()));
  float max_scale = std::max(static_cast<float>(SCREEN_WIDTH) / Sector::get().get_width(),
                             static_cast<float>(SCREEN_HEIGHT) / Sector::get().get_height());

  // Capping at `m_scale` allows fixing a minor bug where the camera would
  // sometimes be slightly off-sector if scaling goes faster than moving.
  scale = math::clamp(scale, max_scale, m_scale);

  // Can't use m_screen_size because it varies depending on the scale
  auto rect = Rectf::from_center(Vector((cover.get_left() + cover.get_right()) / 2.f, (cover.get_top() + cover.get_bottom()) / 2.f),
                                Sizef(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)));
  auto true_rect = Rectf::from_center(Vector((cover.get_left() + cover.get_right()) / 2.f, (cover.get_top() + cover.get_bottom()) / 2.f),
                                      Sizef(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)) / scale);

  if (true_rect.get_left() < 0.f)
    rect.move(Vector(-true_rect.get_left(), 0.f));
  if (true_rect.get_top() < 0.f)
    rect.move(Vector(0.f, -true_rect.get_top()));
  if (true_rect.get_right() > Sector::get().get_width())
    rect.move(Vector(Sector::get().get_width() - true_rect.get_right(), 0.f));
  if (true_rect.get_bottom() > Sector::get().get_height())
    rect.move(Vector(0.f, Sector::get().get_height() - true_rect.get_bottom()));

  m_translation = m_translation * (1.f - MULTIPLAYER_CAM_WEIGHT) + rect.p1() * MULTIPLAYER_CAM_WEIGHT;
  m_minimum_scale = m_minimum_scale * (1.f - MULTIPLAYER_CAM_WEIGHT) + scale * MULTIPLAYER_CAM_WEIGHT;
}

void
Camera::update_scroll_autoscroll(float dt_sec)
{
  if (!get_parent()->get_object_count<Player>([](const Player& p) { return p.is_alive(); }))
    return;

  get_walker()->update(dt_sec);
  // TODO: Get the camera size?
  m_translation = get_walker()->get_pos(Sizef(), m_path_handle);

  keep_in_bounds(m_translation);
}

void
Camera::update_scroll_to(float dt_sec)
{
  m_scroll_to_pos += dt_sec * m_scrollspeed;
  if (m_scroll_to_pos >= 1.0f)
  {
    m_mode = Mode::MANUAL;
    m_translation = m_scroll_goal;

    // If a scale is active and wouldn't finish this frame, reload it with the remaining time,
    // setting the initial scale values from the scroll destination.
    if (m_scale_time_remaining - dt_sec > 0.f)
    {
      m_scale_time_total -= m_scale_time_total - m_scale_time_remaining;
      reload_scale();
    }
    else
    {
      // In case a scale finishes this frame, set its target translation to the scroll destination.
      m_scale_target_translation = m_translation;
    }

    return;
  }

  m_translation = m_scroll_from + (m_scroll_goal - m_scroll_from) * m_scroll_to_pos;
}

void
Camera::update_scale(float dt_sec)
{
  if (m_scale_time_remaining > 0.f)
  {
    m_scale_time_remaining -= dt_sec;

    if (m_scale_time_remaining <= 0.f)
    {
      m_scale = m_scale_target;
      if (m_mode == Mode::MANUAL)
        m_translation = m_scale_target_translation;

      m_scale_time_remaining = 0.f;
      m_scale_time_total = 0.f;
    }
    else
    {
      float time_progress = (m_scale_time_total - m_scale_time_remaining) / m_scale_time_total;
      float progress = static_cast<float>(m_scale_easing(static_cast<double>(time_progress)));

      m_scale = m_scale_origin + (m_scale_target - m_scale_origin) * progress;

      /** MANUAL mode scale management */
      if (m_mode == Mode::MANUAL)
      {
        // Move camera to the target translation, when zooming in manual mode.
        m_translation = m_scale_origin_translation + (m_scale_target_translation - m_scale_origin_translation) * progress;
        keep_in_bounds(m_translation);
        return;
      }
    }

    // Re-center camera, when zooming in normal mode.
    m_lookahead_pos /= 1.01f;
  }

  // In MANUAL mode, the translation is managed only when a scale is active.
  // In SCROLLTO mode, the translation is managed in update_scroll_to().
  if (m_mode == Mode::MANUAL || m_mode == Mode::SCROLLTO)
    return;

  // FIXME: Poor design: This shouldn't pose a problem to multiplayer.
  if (m_mode == Mode::NORMAL && Sector::current()->get_object_count<Player>() > 1)
    return;

  Vector screen_size = m_screen_size.as_vector();
  m_translation += screen_size * (1.f - get_current_scale()) / 2.f;
}

float
Camera::get_current_scale() const
{
  return m_enfore_minimum_scale ? std::min(m_minimum_scale, m_scale) : m_scale;
}

/** Get target scale position from the anchor point (m_scale_anchor). */
Vector
Camera::get_scale_anchor_target() const
{
  // Get target center position from the anchor, and afterwards, top-left position from the center position.
  return get_anchor_center_pos(Rectf(m_translation,
                                     Sizef(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT))),
                               m_scale_anchor) - Vector(static_cast<float>(SCREEN_WIDTH) / 2, static_cast<float>(SCREEN_HEIGHT) / 2);
}

/** Reload easing scale from the current position. */
void
Camera::reload_scale()
{
  m_scale_origin = m_scale;
  m_scale_origin_translation = m_translation;
  m_scale_target_translation = get_scale_anchor_target();
  m_scale_time_remaining = m_scale_time_total;
}

void
Camera::ease_scale(float scale, float time, easing ease, AnchorPoint anchor)
{
  if (m_scale == scale && m_scale_target == scale)
    return;

  m_scale_target = scale;
  m_scale_time_total = time;
  m_scale_easing = ease;
  m_scale_anchor = anchor;

  reload_scale();

  if (time <= 0.f)
  {
    m_scale = scale;
    if (m_mode == Mode::MANUAL)
      m_translation = m_scale_target_translation;
  }
}

void
Camera::move(float x, float y)
{
  scroll_to(m_translation + Vector(x, y), 0.0f);
}

void
Camera::set_mode(const std::string& mode)
{
  if (mode == "normal")
    m_mode = Mode::NORMAL;
  else if (mode == "manual")
    m_mode = Mode::MANUAL;
  else
    log_warning << "Camera mode '" << mode << "' unknown." << std::endl;
}

void
Camera::set_scale(float scale, float time, int anchor, const std::string& ease)
{
  ease_scale(scale, time, getEasingByName(EasingMode_from_string(ease)), static_cast<AnchorPoint>(anchor));
}

Vector
Camera::get_center() const
{
  return m_translation + Vector(m_screen_size.width / 2.0f,
                                m_screen_size.height / 2.0f);
}

const Vector&
Camera::get_position() const
{
  return m_translation;
}

const Sizef&
Camera::get_screen_size() const
{
  return m_screen_size;
}


void
Camera::move(const Vector& offset)
{
  scroll_to(m_translation + offset, 0.0f);
}

bool
Camera::is_saveable() const
{
  return !(Level::current() &&
           Level::current()->is_worldmap());
}


void
Camera::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Camera>("Camera", vm.findClass("GameObject"));

  PathObject::register_members(cls);

  cls.addFunc("shake", &Camera::shake);
  cls.addFunc("start_earthquake", &Camera::start_earthquake);
  cls.addFunc("stop_earthquake", &Camera::stop_earthquake);
  cls.addFunc("set_pos", &Camera::set_pos);
  cls.addFunc<void, Camera, float, float>("move", &Camera::move);
  cls.addFunc<void, Camera, const std::string&>("set_mode", &Camera::set_mode);
  cls.addFunc<void, Camera, float, float, float>("scroll_to", &Camera::scroll_to);
  cls.addFunc("get_current_scale", &Camera::get_current_scale);
  cls.addFunc("get_target_scale", &Camera::get_target_scale);
  cls.addFunc("set_scale", &Camera::set_scale, ssq::DefaultArguments<float, int, const std::string&>(0.f, AnchorPoint::ANCHOR_MIDDLE, ""));
  cls.addFunc("set_scale_anchor", &Camera::set_scale_anchor); // Deprecated
  cls.addFunc("scale", &Camera::scale); // Deprecated
  cls.addFunc("scale_anchor", &Camera::scale_anchor); // Deprecated
  cls.addFunc<void, Camera, float, float, const std::string&>("ease_scale", &Camera::ease_scale); // Deprecated
  cls.addFunc("ease_scale_anchor", &Camera::set_scale); // Deprecated
  cls.addFunc("get_screen_width", &Camera::get_screen_width);
  cls.addFunc("get_screen_height", &Camera::get_screen_height);
  cls.addFunc("get_x", &Camera::get_x);
  cls.addFunc("get_y", &Camera::get_y);
}

/* EOF */
