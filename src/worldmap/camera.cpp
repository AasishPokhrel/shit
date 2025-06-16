//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "worldmap/camera.hpp"

#include "supertux/globals.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"
#include "worldmap/tux.hpp"
#include "worldmap/worldmap.hpp"
#include "math/util.hpp"

namespace {

const float CAMERA_PAN_TIME_MAX = 0.52213f;

} // namespace

namespace worldmap {

Camera::Camera(WorldMapSector& worldmap_sector) :
  m_worldmap_sector(worldmap_sector),
  m_camera_offset(0.0f, 0.0f),
  m_pan_startpos(0.0f, 0.0f),
  m_pan_time_full(0),
  m_pan_time_remaining(0),
  m_panning(false)
{
}

void
Camera::update(float dt_sec)
{
  // Position the "camera".
  Vector target_pos = get_camera_pos_for_tux();
  clamp_camera_position(target_pos);
  if (!m_panning) {
    m_camera_offset = target_pos;
    return;
  }

  m_pan_time_remaining -= dt_sec;
  if (m_pan_time_remaining > 0) {
    // Smoothly interpolate the camera's position.
    float f = m_pan_time_remaining / m_pan_time_full;
    f = 0.5f - 0.5f * cosf(math::PI * f);
    m_camera_offset.x = f * m_pan_startpos.x + (1.0f - f) * target_pos.x;
    m_camera_offset.y = f * m_pan_startpos.y + (1.0f - f) * target_pos.y;
    return;
  }

  m_camera_offset = target_pos;
  m_panning = false;
}

void
Camera::pan()
{
  m_panning = true;
  m_pan_startpos = m_camera_offset;
  Vector target_pos = get_camera_pos_for_tux();
  clamp_camera_position(target_pos);
  Vector start_to_target = target_pos - m_pan_startpos;
  m_pan_time_full = glm::length(start_to_target) / 612.41f;
  if (m_pan_time_full > CAMERA_PAN_TIME_MAX)
    m_pan_time_full = CAMERA_PAN_TIME_MAX;
  m_pan_time_remaining = m_pan_time_full;
}

Vector
Camera::get_camera_pos_for_tux() const
{
  auto& tux = m_worldmap_sector.get_singleton_by_type<Tux>();

  Vector camera_offset_(0.0f, 0.0f);
  Vector tux_pos = tux.get_pos();
  camera_offset_.x = tux_pos.x - static_cast<float>(SCREEN_WIDTH) / 2.0f;
  camera_offset_.y = tux_pos.y - static_cast<float>(SCREEN_HEIGHT) / 2.0f;
  return camera_offset_;
}

void
Camera::clamp_camera_position(Vector& c) const
{
  if (c.x < 0) {
    c.x = 0;
  }

  if (c.y < 0) {
    c.y = 0;
  }

  if (c.x > m_worldmap_sector.get_width() - static_cast<float>(SCREEN_WIDTH)) {
    c.x = m_worldmap_sector.get_width() - static_cast<float>(SCREEN_WIDTH);
  }

  if (c.y > m_worldmap_sector.get_height() - static_cast<float>(SCREEN_HEIGHT)) {
    c.y = m_worldmap_sector.get_height() - static_cast<float>(SCREEN_HEIGHT);
  }

  if (m_worldmap_sector.get_width() < static_cast<float>(SCREEN_WIDTH)) {
    c.x = (m_worldmap_sector.get_width() - static_cast<float>(SCREEN_WIDTH)) / 2.0f;
  }

  if (m_worldmap_sector.get_height() < static_cast<float>(SCREEN_HEIGHT)) {
    c.y = (m_worldmap_sector.get_height() - static_cast<float>(SCREEN_HEIGHT)) / 2.0f;
  }
}

} // namespace worldmap

/* EOF */
