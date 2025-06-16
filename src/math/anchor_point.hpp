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

#ifndef HEADER_SUPERTUX_MATH_ANCHOR_POINT_HPP
#define HEADER_SUPERTUX_MATH_ANCHOR_POINT_HPP

#include <string>
#include <vector>

#include "math/vector.hpp"

class Rectf;

#ifdef DOXYGEN_SCRIPTING
/**
 * @scripting
 * @summary This module contains global anchor constants.
 */
namespace AnchorPoints {
#endif

/**
 * @scripting
 */
enum AnchorPoint {
  ANCHOR_TOP_LEFT = 0,     /*!< @description Top-left anchor point. */
  ANCHOR_TOP = 1,          /*!< @description Top anchor point. */
  ANCHOR_TOP_RIGHT = 2,    /*!< @description Top-right anchor point. */
  ANCHOR_LEFT = 3,         /*!< @description Left anchor point. */
  ANCHOR_MIDDLE = 4,       /*!< @description Middle anchor point. */
  ANCHOR_RIGHT = 5,        /*!< @description Right anchor point. */
  ANCHOR_BOTTOM_LEFT = 6,  /*!< @description Bottom-left anchor point. */
  ANCHOR_BOTTOM = 7,       /*!< @description Bottom anchor point. */
  ANCHOR_BOTTOM_RIGHT = 8, /*!< @description Bottom-right anchor point. */
  ANCHOR_LAST = ANCHOR_BOTTOM_RIGHT
};

#ifdef DOXYGEN_SCRIPTING
} // namespace AnchorPoints
#endif

const std::vector<std::string> g_anchor_keys = {
  "topleft", "top", "topright", "left", "middle", "right", "bottomleft",
  "bottom", "bottomright"
};

std::vector<std::string> get_anchor_names();
std::string anchor_point_to_string(AnchorPoint point);
AnchorPoint string_to_anchor_point(const std::string& str);
Vector get_anchor_pos(const Rectf& rect, AnchorPoint point);
Vector get_anchor_pos(const Rectf& destrect, float width, float height,
                      AnchorPoint point);
Vector get_anchor_center_pos(const Rectf& rect, AnchorPoint point);

#endif

/* EOF */
