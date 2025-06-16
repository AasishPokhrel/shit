//  SuperTux
//  Copyright (C) 2013 LMH <lmh.0013@gmail.com>
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

#include "object/coin_rain.hpp"

#include "math/random.hpp"
#include "object/coin.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

static const float DROP_TIME = .1f; // Time duration between "drops" of coin rain.

CoinRain::CoinRain(const Vector& pos, bool emerge, bool count_stats, const std::string& sprite_path) :
  sprite(SpriteManager::current()->create(sprite_path)),
  m_sprite_path(sprite_path),
  position(pos),
  emerge_distance(0),
  timer(),
  counter(0),
  drop(0),
  m_count_stats(count_stats)
{
  if (emerge) {
    emerge_distance = static_cast<float>(sprite->get_height());
  }
}

void
CoinRain::update(float dt_sec)
{
  // First a single (untouchable) coin flies up above the sector.
  if (position.y > -32){
    float dist = -500 * dt_sec;
    position.y += dist;
    emerge_distance += dist;
  } // Then the first collectable coin drops from one of ten random positions.
  else if (counter==0){
    drop = gameRandom.rand(10);
    Sector::get().add<HeavyCoin>(Vector(position.x + 32.0f * static_cast<float>((drop < 5) ? -drop - 1 : drop - 4), -32.0f),
                                                              Vector(0, 0), m_count_stats, m_sprite_path);
    counter++;
    timer.start(DROP_TIME);
  } // Finally, the remaining coins drop in a determined but seemingly random order.
  else if (timer.check()){
    if (counter<10){
      drop += 7;
      if (drop >= 10) drop -=10;
      Sector::get().add<HeavyCoin>(Vector(position.x + 32.0f * static_cast<float>((drop < 5) ? -drop - 1 : drop - 4), -32.0f),
                                                                Vector(0, 0), m_count_stats, m_sprite_path);
      counter++;
      timer.start(DROP_TIME);
    } else {
      remove_me();
    }
  }
}

void
CoinRain::draw(DrawingContext& context)
{
  int layer;
  if (emerge_distance > 0) {
    layer = LAYER_OBJECTS - 5;
  } else {
    layer = LAYER_OBJECTS + 5;
  }
  sprite->draw(context.color(), position, layer);
}

/* EOF */
