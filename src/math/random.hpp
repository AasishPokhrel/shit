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

#ifndef HEADER_SUPERTUX_MATH_RANDOM_HPP
#define HEADER_SUPERTUX_MATH_RANDOM_HPP

#include <random>

class Random
{
public:
  Random();

  /** Seed the generator */
  void seed(int v);

  /** Generate random integers between [0, INT_MAX) */
  int rand();

  /** Generate random integers between [0, v) */
  int rand(int v);

  /** Generate random integers between [u, v) */
  int rand(int u, int v);

  /** Generate random floats between [0, v) */
  float randf(float v);

  /** Generate random floats between [u, v) */
  float randf(float u, float v);

private:
  std::mt19937 m_generator;

private:
  Random(const Random&) = delete;
  Random& operator=(const Random&) = delete;
};

/** Use for random particle fx or whatever */
extern Random graphicsRandom;

/** Use for game-changing random numbers */
extern Random gameRandom;

#endif

/* EOF */
