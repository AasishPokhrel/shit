//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
//                2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_WORLDMAP_CAMERA_HPP
#define HEADER_SUPERTUX_WORLDMAP_CAMERA_HPP

#include "math/vector.hpp"

namespace worldmap {

class WorldMapSector;

class Camera
{
public:
  explicit Camera(WorldMapSector& worldmap_sector);

  void update(float dt_sec);

  void pan();
  inline bool is_panning() const { return m_panning; }

  inline Vector get_offset() const { return m_camera_offset; }

private:
  Vector get_camera_pos_for_tux() const;
  void clamp_camera_position(Vector& c) const;

private:
  WorldMapSector& m_worldmap_sector;
  Vector m_camera_offset;

  /** variables to track panning to a spawn point */
  Vector m_pan_startpos;
  float m_pan_time_full;
  float m_pan_time_remaining;
  bool m_panning;

private:
  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
