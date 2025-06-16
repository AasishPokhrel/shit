//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
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

#include "badguy/walking_candle.hpp"

#include "object/lantern.hpp"
#include "sprite/sprite.hpp"
#include "util/reader_mapping.hpp"

WalkingCandle::WalkingCandle(const ReaderMapping& reader)
  : WalkingBadguy(reader, "images/creatures/mr_candle/mr-candle.sprite", "left", "right"),
    m_lightcolor(1, 1, 1)
{
  walk_speed = 80;
  max_drop_height = 64;

  std::vector<float> vColor;
  if (reader.get("color", vColor))
    m_lightcolor = Color(vColor);

  m_sprite->set_color(m_lightcolor);
  m_lightsprite->set_color(m_lightcolor);

  m_countMe = false;
  m_glowing = true;
}

bool
WalkingCandle::is_freezable() const
{
  return true;
}

bool
WalkingCandle::is_flammable() const
{
  return m_frozen;
}

void
WalkingCandle::freeze()
{
  BadGuy::freeze();
  m_glowing = false;
}

void
WalkingCandle::unfreeze(bool melt)
{
  BadGuy::unfreeze(melt);
  initialize();
  m_sprite->set_color(m_lightcolor);
  m_lightsprite->set_color(m_lightcolor);
  m_glowing = true;
}

HitResponse
WalkingCandle::collision(MovingObject& other, const CollisionHit& hit)
{
  auto lantern = dynamic_cast<Lantern*>(&other);
  if (lantern && !m_frozen) if (lantern->get_bbox().get_bottom() < m_col.m_bbox.get_top())
  {
    lantern->add_color(m_lightcolor);
    run_dead_script();
    remove_me();
    return FORCE_MOVE;
  }
  return WalkingBadguy::collision(other, hit);
}

void
WalkingCandle::kill_fall()
{
  if (!m_frozen)
    return;
  unfreeze(false);
}

ObjectSettings
WalkingCandle::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_color(_("Color"), &m_lightcolor, "color", Color::WHITE);
  result.reorder({"color", "x", "y"});

  return result;
}

void
WalkingCandle::after_editor_set()
{
  WalkingBadguy::after_editor_set();

  m_sprite->set_color(m_lightcolor);
  m_lightsprite->set_color(m_lightcolor);
}

/* EOF */
