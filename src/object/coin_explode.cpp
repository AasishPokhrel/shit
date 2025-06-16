//  CoinExplode - several coins are hurled through the air
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

#include "object/coin_explode.hpp"

#include "math/random.hpp"
#include "object/coin.hpp"
#include "supertux/sector.hpp"

#include <list>

CoinExplode::CoinExplode(const Vector& pos, bool count_stats, const std::string& sprite) :
  m_sprite(sprite),
  position(pos),
  m_count_stats(count_stats)
{
}

void
CoinExplode::update(float )
{
  float mag = 100.0f; // Magnitude at which coins are thrown.
  float rand = 30.0f; // Max variation to be subtracted from the magnitude.

  // Each coin in the explosion has a different velocity.
  static std::list<Vector> coin_velocities = {
    { 2.5, -4.5 }, { 2, -5 },
    { 1.5, -5.5 }, { 1, -6 }, { 0.5, -6.5 },
    { -2.5, -4.5 }, { -2, -5 },
    { -1.5, -5.5 }, { -1, -6 }, { -0.5, -6.5 }
  };

  for(const auto& vector: coin_velocities)
  {
    auto velocity = vector * (mag - gameRandom.randf(rand));
    Sector::get().add<HeavyCoin>(position, velocity, m_count_stats, m_sprite);
  }

  remove_me();
}

void
CoinExplode::draw(DrawingContext &)
{
}

/* EOF */
