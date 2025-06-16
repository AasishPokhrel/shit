//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
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

#ifndef HEADER_SUPERTUX_WORLDMAP_WORLD_SELECT_HPP
#define HEADER_SUPERTUX_WORLDMAP_WORLD_SELECT_HPP

#include <memory>

#include "supertux/screen.hpp"
#include "util/currenton.hpp"
#include "video/surface_ptr.hpp"

namespace worldmap {

class WorldMap;

class WorldSelect final : public Screen,
                          public Currenton<WorldSelect>
{
private:
  static const float s_torque;

private:
  class WMdata
  {
  public:
    WMdata() = default;

    std::string filename = "";
    std::string name = "";
    bool unlocked = false;
    SurfacePtr icon = nullptr;
  };

public:
  WorldSelect(const std::string& current_world_filename);
  ~WorldSelect() override;

  virtual void setup() override;
  virtual void leave() override;

  virtual void draw(Compositor& compositor) override;
  virtual void update(float dt_sec, const Controller& controller) override;
  virtual IntegrationStatus get_status() const override;

private:
  bool m_enabled;
  std::vector<WMdata> m_worlds;
  int m_current_world;
  int m_selected_world;
  float m_angle;
  SurfacePtr m_bkg;

private:
  WorldSelect(const WorldSelect&) = delete;
  WorldSelect& operator=(const WorldSelect&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
