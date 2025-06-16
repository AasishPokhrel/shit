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

#include "math/util.hpp"
#include "video/color.hpp"

#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <regex>
#include <sstream>

const Color Color::BLACK(0.0, 0.0, 0.0);
const Color Color::RED(1.0, 0.0, 0.0);
const Color Color::GREEN(0.0, 1.0, 0.0);
const Color Color::BLUE(0.0, 0.0, 1.0);
const Color Color::CYAN(0.0, 1.0, 1.0);
const Color Color::MAGENTA(1.0, 0.0, 1.0);
const Color Color::YELLOW(1.0, 1.0, 0.0);
const Color Color::WHITE(1.0, 1.0, 1.0);

Color::Color() :
  red(0),
  green(0),
  blue(0),
  alpha(1.0f)
{}

Color::Color(float red_, float green_, float blue_, float alpha_) :
  red(red_),
  green(green_),
  blue(blue_),
  alpha(alpha_)
{
  red = std::clamp(red, 0.f, 1.f);
  green = std::clamp(green, 0.f, 1.f);
  blue = std::clamp(blue, 0.f, 1.f);
  alpha = std::clamp(alpha, 0.f, 1.f);
}

Color::Color(const std::vector<float>& vals, bool use_alpha) :
  red(),
  green(),
  blue(),
  alpha()
{
  if (vals.size() < 3) {
    red = 0;
    green = 0;
    blue = 0;
    alpha = 0;
    return;
  }

  red   = vals[0];
  green = vals[1];
  blue  = vals[2];
  if (use_alpha && vals.size() > 3)
    alpha = vals[3];
  else
    alpha = 1.0;

  red = std::clamp(red, 0.f, 1.f);
  green = std::clamp(green, 0.f, 1.f);
  blue = std::clamp(blue, 0.f, 1.f);
}

bool
Color::operator==(const Color& other) const
{
  return red == other.red && green == other.green && blue == other.blue
    && alpha == other.alpha;
}

bool
Color::operator!=(const Color& other) const
{
  return !(operator==(other));
}

float
Color::greyscale() const
{
  return red * 0.30f + green * 0.59f + blue * 0.11f;
}

Color
Color::multiply_linearly(float v) const
{
  // For the approximate sRGB conversion, it is sufficient to apply the exponent
  // to v.
  v = add_gamma(v);
  return Color(red * v, green * v, blue * v, alpha);
}

bool
Color::operator < (const Color& other) const
{
  return greyscale() < other.greyscale();
}

std::vector<float>
Color::toVector()
{
  std::vector<float> result;
  result.clear();
  result.push_back(red);
  result.push_back(green);
  result.push_back(blue);
  result.push_back(alpha);
  return result;
}

std::optional<Color>
Color::deserialize_from_rgb(const std::string & rgb_string)
{
  const std::regex rgb_format(R"(^\s*rgb\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)\s*$)");
  std::smatch matches;

  if (std::regex_match(rgb_string, matches, rgb_format))
  {
    const int r = std::stoi(matches[1].str());
    const int g = std::stoi(matches[2].str());
    const int b = std::stoi(matches[3].str());

    if (math::in_bounds(r, 0, 255) && math::in_bounds(g, 0, 255) && math::in_bounds(b, 0, 255))
    {
      return Color(static_cast<float>(r) / 255.0f,
                   static_cast<float>(g) / 255.0f,
                   static_cast<float>(b) / 255.0f,
                   1.0f);
    }
  }
  return std::nullopt;
}

std::optional<Color>
Color::deserialize_from_hex(const std::string& hex_string)
{
  const std::regex hex_format(R"(^\s*#([A-Fa-f0-9]{6})\s*$)");
  std::smatch matches;

  if (std::regex_match(hex_string, matches, hex_format))
  {
    const std::string hex_value = matches[1].str();
    unsigned int hex_color;
    std::stringstream ss;
    ss << std::hex << hex_value;
    ss >> hex_color;

    const float r = ((hex_color >> 16) & 0xFF) / 255.0f;
    const float g = ((hex_color >> 8) & 0xFF) / 255.0f;
    const float b = (hex_color & 0xFF) / 255.0f;

    return Color(r, g, b, 1.0f);
  }
  return std::nullopt;
}

std::string
Color::serialize_to_hex(const Color& color)
{
  std::stringstream ss;
  ss << "#"
     << std::hex << std::setfill('0') << std::uppercase
     << std::setw(2) << static_cast<int>(color.red * 255.f)
     << std::setw(2) << static_cast<int>(color.green * 255.f)
     << std::setw(2) << static_cast<int>(color.blue * 255.f);
  return ss.str();
}

std::string
Color::serialize_to_rgb(const Color& color)
{
  std::stringstream ss;
  ss << "rgb("
     << static_cast<int>(color.red * 255.f) << ","
     << static_cast<int>(color.green * 255.f) << ","
     << static_cast<int>(color.blue * 255.f) << ")";
  return ss.str();
}

/* EOF */
