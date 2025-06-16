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

#include "worldmap/world_select.hpp"

#include <algorithm>

#include "control/controller.hpp"
#include "math/util.hpp"
#include "squirrel/squirrel_virtual_machine.hpp"
#include "supertux/constants.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/resources.hpp"
#include "supertux/screen_manager.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "video/compositor.hpp"
#include "video/drawing_context.hpp"
#include "video/surface.hpp"
#include "worldmap/worldmap.hpp"

namespace worldmap {

const float WorldSelect::s_torque = 0.75f;

WorldSelect::WorldSelect(const std::string& current_world_filename) :
  m_enabled(false),
  m_worlds(),
  m_current_world(),
  m_selected_world(),
  m_angle(),
  m_bkg()
{
  auto& vm = SquirrelVirtualMachine::current()->get_vm();

  int i = 0;
  try
  {
    ssq::Table world_select = vm.findTable("state").findTable("world_select");
    std::vector<std::string> worlds = world_select.getKeys();

    if (!worlds.empty())
      std::reverse(worlds.begin(), worlds.end());

    // Only worlds with a set prefix, which also are numbered starting with 1, will be ordered properly.
    // This is a probably a poor solution, but I can't think of any other. - Daniel
    std::string prefix;
    world_select.get("prefix", prefix);
    if (!prefix.empty())
    {
      for (size_t y = 0; y < worlds.size(); y++)
      {
        worlds[y] = prefix + std::to_string(y + 1) + "/worldmap.stwm";
      }
    }

    for (const std::string& world : worlds)
    {
      try
      {
        ssq::Table world_table = world_select.findTable(world.c_str());

        bool unlocked = false;
        world_table.get("unlocked", unlocked);

        WMdata wm;
        wm.filename = world;
        wm.unlocked = unlocked;

        ReaderDocument doc = ReaderDocument::from_file(world);
        if (!doc.get_root().get_mapping().get("name", wm.name))
        {
          log_warning << "No name for worldmap " << world << std::endl;
          continue;
        }

        std::string icon_path = "";
        if (!doc.get_root().get_mapping().get(unlocked ? "icon" : "icon-locked", icon_path))
        {
          log_warning << "No icon (" << (unlocked ? "unlocked" : "locked") << ") for worldmap " << world << std::endl;
          continue;
        }

        wm.icon = Surface::from_file(icon_path);
        if (!wm.icon)
        {
          log_warning << "Icon not found for worldmap " << world << ": "
                      << icon_path << std::endl;
          continue;
        }

        m_worlds.push_back(wm);
        if (current_world_filename == world)
        {
          m_current_world = i;

          std::string bkg_path = "";
          if (doc.get_root().get_mapping().get("bkg", bkg_path))
          {
            m_bkg = Surface::from_file(bkg_path);
          }
          else
          {
            m_bkg = Surface::from_file("/images/worlds/background/default.png");
          }
        }
        i++;
      }
      catch(const std::exception& e)
      {
        log_info << "Exception thrown while generating world state: " << e.what() << std::endl;
      }
    }
  }
  catch (const std::exception& err)
  {
    log_warning << "Couldn't load world select data: " << err.what() << std::endl;
  }

  m_selected_world = m_current_world;
  m_angle = static_cast<float>(m_current_world) / static_cast<float>(i) * math::PI * 2;

  if (m_worlds.empty())
  {
    log_warning << "No maps on world select" << std::endl;
  }
}

WorldSelect::~WorldSelect()
{
}

void
WorldSelect::setup()
{
  if (m_worlds.empty())
    ScreenManager::current()->pop_screen(std::make_unique<FadeToBlack>(FadeToBlack::Direction::FADEOUT, 0.25f));

  m_enabled = true;
}

void
WorldSelect::leave()
{
  m_enabled = false;
}

void
WorldSelect::draw(Compositor& compositor)
{
  auto& context = compositor.make_context();
  context.color().draw_filled_rect(context.get_rect(), Color(), -1000);
  context.color().draw_surface_scaled(m_bkg, context.get_viewport(), -999);

  std::string name_to_display;
  float distance = 0.f;

  int i = 0;
  for (const auto& world : m_worlds)
  {
    float angle = m_angle - static_cast<float>(i) /
                  static_cast<float>(m_worlds.size()) * math::PI * 2;

    float size = 1.f + (std::cos(angle) - 1.f) / 4.f;
    Rectf rect = world.icon->get_region();
    rect = Rectf(0, 0, rect.get_width() * size / 2.f, rect.get_height() * size / 2.f);
    rect.move(Vector(context.get_width() / 2.f - rect.get_width() / 2.f,
                     context.get_height() / 2.f - rect.get_height() / 2.f));
    rect.move(Vector(std::sin(angle) * -context.get_width() / 4.f, 0.f));

    PaintStyle ps;
    ps.set_alpha(std::cos(angle) * .5f + .5f);
    context.color().draw_surface_scaled(world.icon, rect,
                                        static_cast<int>(rect.get_bottom()), ps);

    if (std::cos(angle) > distance)
    {
      distance = std::cos(angle);
      name_to_display = world.unlocked ? world.name : "???";
    }

    i++;
  }

  float halfangle = 1.f / static_cast<float>(m_worlds.size()) * math::PI * 2;
  float o = distance * (.5f - std::cos(halfangle));
  context.color().draw_text(Resources::big_font, name_to_display,
                            Vector(context.get_width() / 2.f,
                                   context.get_height() * 3.f / 4.f + pow(10.f - o * 10.f, 2.f)),
                            FontAlignment::ALIGN_CENTER,
                            10,
                            Color(1.f, 1.f, 1.f,static_cast<float>(pow(o, 2.f)) * 4.f));
}

void
WorldSelect::update(float dt_sec, const Controller& controller)
{
  float target = static_cast<float>(m_selected_world) /
                 static_cast<float>(m_worlds.size()) * math::PI * 2;

  while (m_angle - target > math::PI)
    target += math::PI * 2;

  while (m_angle - target < -math::PI)
    target -= math::PI * 2;

  m_angle = m_angle * s_torque + target * (1.f - s_torque);

  if (!m_enabled)
    return;

  if (controller.pressed_any(Control::ESCAPE, Control::ACTION))
  {
    m_enabled = false;
    ScreenManager::current()->pop_screen(std::make_unique<FadeToBlack>(FadeToBlack::Direction::FADEOUT, 0.25f));
    return;
  }

  if (controller.pressed(Control::LEFT)) {
    m_selected_world--;
    // Modulo doesn't work for some reason
    if (m_selected_world < 0)
    {
      m_selected_world += static_cast<int>(m_worlds.size());
    }
  }

  if (controller.pressed(Control::RIGHT)) {
    m_selected_world++;
    m_selected_world %= static_cast<int>(m_worlds.size());
  }

  if (controller.pressed(Control::JUMP) && m_worlds[m_selected_world].unlocked) {
    m_enabled = false;
    ScreenManager::current()->pop_screen(std::make_unique<FadeToBlack>(FadeToBlack::Direction::FADEOUT, 0.25f));
    worldmap::WorldMap::current()->change(m_worlds[m_selected_world].filename, "", DEFAULT_SPAWNPOINT_NAME);
    return;
  }
}

IntegrationStatus
WorldSelect::get_status() const
{
  IntegrationStatus status;
  status.m_details.push_back("In world select");
  if (!m_worlds.empty())
  {
    status.m_details.push_back(m_worlds[m_current_world].name);
  }
  return status;
}

} // namespace worldmap

/* EOF */
