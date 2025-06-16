//  SuperTux
//  Copyright (C) 2023 MatusGuy
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

#include "badguy/granito_big.hpp"

#include "util/reader_mapping.hpp"
#include "supertux/sector.hpp"

GranitoBig::GranitoBig(const ReaderMapping& reader) :
  Granito(reader, "images/creatures/granito/big/granito_big.sprite", LAYER_OBJECTS - 2),
  m_carrying(nullptr)
{
  parse_type(reader);

  reader.get("carrying-script", m_carried_script);

  max_drop_height = 16;
}

HitResponse
GranitoBig::collision_player(Player& player, const CollisionHit& hit)
{
  // Prevent from triggering STATE_LOOKUP
  return FORCE_MOVE;
}

void
GranitoBig::active_update(float dt_sec)
{
  Granito::active_update(dt_sec);

  if (!m_carrying)
    return;

  Vector pos(get_bbox().get_middle().x - m_carrying->get_bbox().get_width() / 2,
             get_bbox().get_top() - m_carrying->get_bbox().get_height());
  m_carrying->set_pos(pos);
  m_carrying->turn(m_dir);
}

ObjectSettings
GranitoBig::get_settings()
{
  auto settings = Granito::get_settings();

  settings.remove("detect-script");
  settings.remove("carried-script");

  // No need to make another member for the carrying script.
  // Just repurpose the carried script.
  settings.add_script(_("Carrying Script"), &m_carried_script, "carrying-script");

  return settings;
}

GameObjectTypes
GranitoBig::get_types() const
{
  return {
    { "default", _("Default") },
    { "standing", _("Standing") },
    { "walking", _("Walking") },
    { "scriptable", _("Scriptable") },
  };
}

void
GranitoBig::carry(Granito* granito)
{
  m_carrying = granito;
  Sector::get().run_script(m_carried_script, "carrying-script");
}

void
GranitoBig::eject()
{
  if (!m_carrying)
    return;

  m_carrying->walk_for(1.5f);
  m_carrying->jump();
  m_carrying = nullptr;
}

std::string
GranitoBig::get_carrying_name() const
{
  return m_carrying ? m_carrying->get_name() : "";
}

bool
GranitoBig::try_wave()
{
  return false;
}

bool
GranitoBig::try_jump()
{
  return false;
}

void
GranitoBig::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<GranitoBig>("GranitoBig", vm.findClass("Granito"));

  cls.addFunc("get_carrying_name", &GranitoBig::get_carrying_name);
}

/* EOF */
