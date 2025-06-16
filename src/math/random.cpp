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

#include "math/random.hpp"

#include <ctime>
#include <limits>

Random graphicsRandom;
Random gameRandom;

Random::Random() :
  m_generator()
{
}

void
Random::seed(int v)
{
  if (v <= 0)
    v = static_cast<int>(std::time(nullptr)); // Use the UNIX timestamp of the current time as a seed.

  m_generator.seed(v);
}

int
Random::rand()
{
  std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max() - 1);
  return dist(m_generator);
}

int
Random::rand(int v)
{
  std::uniform_int_distribution<int> dist(0, v - 1);
  return dist(m_generator);
}

int
Random::rand(int u, int v)
{
  std::uniform_int_distribution<int> dist(u, v - 1);
  return dist(m_generator);
}

float
Random::randf(float v)
{
  std::uniform_real_distribution<float> dist(0.0f, v);
  return dist(m_generator);
}

float
Random::randf(float u, float v)
{
  std::uniform_real_distribution<float> dist(u, v);
  return dist(m_generator);
}

/* EOF */
