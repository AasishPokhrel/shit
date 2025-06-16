//  SuperTux
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

#include "object/smoke_cloud.hpp"

#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"

SmokeCloud::SmokeCloud(const Vector& pos) :
  sprite(SpriteManager::current()->create("images/particles/stomp.sprite")),
  timer(),
  position(pos)
{
  timer.start(.3f);
}

void
SmokeCloud::update(float dt_sec)
{
  position.y -= 120 * dt_sec;

  if (timer.check())
    remove_me();
}

void
SmokeCloud::draw(DrawingContext& context)
{
  Vector draw_pos = position - Vector(0.0, 120.0) * std::min(context.get_time_offset(), timer.get_timeleft());
  sprite->draw(context.color(), draw_pos, LAYER_OBJECTS + 1);
}

/* EOF */
