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

#ifndef HEADER_SUPERTUX_VIDEO_COLOR_HPP
#define HEADER_SUPERTUX_VIDEO_COLOR_HPP

#include <math.h>
#include <string>
#include <optional>
#include <vector>

#include <SDL_image.h>

class Color final
{
public:
  // CalculateColor is the same as Color but without validation
  class CalculateColor final
  {
  public:
    CalculateColor(float r_, float g_, float b_, float a_ = 1.f) :
      r(r_),
      g(g_),
      b(b_),
      a(a_)
    {
    }

    inline Color validate() const { return Color(r, g, b, a); }

    CalculateColor operator+(const CalculateColor& o) const { return CalculateColor(r + o.r, g + o.g, b + o.b, a + o.a); }
    CalculateColor operator-(const CalculateColor& o) const { return CalculateColor(r - o.r, g - o.g, b - o.b, a - o.a); }
    CalculateColor operator*(const CalculateColor& o) const { return CalculateColor(r * o.r, g * o.g, b * o.b, a * o.a); }
    CalculateColor operator/(const CalculateColor& o) const { return CalculateColor(r / o.r, g / o.g, b / o.b, a / o.a); }
    CalculateColor operator+(const Color& o) const { return CalculateColor(r + o.red, g + o.green, b + o.blue, a + o.alpha); }
    CalculateColor operator-(const Color& o) const { return CalculateColor(r - o.red, g - o.green, b - o.blue, a - o.alpha); }
    CalculateColor operator*(const Color& o) const { return CalculateColor(r * o.red, g * o.green, b * o.blue, a * o.alpha); }
    CalculateColor operator/(const Color& o) const { return CalculateColor(r / o.red, g / o.green, b / o.blue, a / o.alpha); }
    CalculateColor operator*(float m) const { return CalculateColor(r * m, g * m, b * m, a * m); }
    CalculateColor operator/(float d) const { return CalculateColor(r / d, g / d, b / d, a / d); }

  public:
    float r, g, b, a;
  };

public:
  static const Color BLACK;
  static const Color RED;
  static const Color GREEN;
  static const Color BLUE;
  static const Color CYAN;
  static const Color MAGENTA;
  static const Color YELLOW;
  static const Color WHITE;

public:
  static Color from_rgb888(uint8_t r, uint8_t g, uint8_t b)
  {
    return Color(static_cast<float>(r) / 255.0f,
                 static_cast<float>(g) / 255.0f,
                 static_cast<float>(b) / 255.0f);
  }

  static Color from_rgba8888(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
  {
    return Color(static_cast<float>(r) / 255.0f,
                 static_cast<float>(g) / 255.0f,
                 static_cast<float>(b) / 255.0f,
                 static_cast<float>(a) / 255.0f);
  }

  static std::optional<Color> deserialize_from_rgb(const std::string& rgb_string);
  static std::optional<Color> deserialize_from_hex(const std::string& hex_string);

  static std::string serialize_to_rgb(const Color& color);
  static std::string serialize_to_hex(const Color& color);

  static Color from_linear(float r, float g, float b, float a = 1.0f)
  {
    return Color(add_gamma(r), add_gamma(g), add_gamma(b), a);
  }

  // Helper functions to approximately transform to/from sRGB colours
  static float add_gamma(float x) { return powf(x, 1.0f / 2.2f); }
  static float remove_gamma(float x) { return powf(x, 2.2f); }

public:
  Color();

  Color(float red_, float green_, float blue_, float alpha_ = 1.0);

  Color(const std::vector<float>& vals, bool use_alpha = true);

  bool operator==(const Color& other) const;
  bool operator!=(const Color& other) const;

  float greyscale() const;

  // Multiplies the sRGB color values by v gamma-correctly
  Color multiply_linearly(float v) const;

  bool operator < (const Color& other) const;

  std::vector<float> toVector();

  inline uint8_t r8() const { return static_cast<uint8_t>(255.0f * red); }
  inline uint8_t g8() const { return static_cast<uint8_t>(255.0f * green); }
  inline uint8_t b8() const { return static_cast<uint8_t>(255.0f * blue); }
  inline uint8_t a8() const { return static_cast<uint8_t>(255.0f * alpha); }

  inline uint32_t rgba() const
  {
    return ((static_cast<uint32_t>(a8()) << 24u) |
            (static_cast<uint32_t>(b8()) << 16u) |
            (static_cast<uint32_t>(g8()) <<  8u) |
            (static_cast<uint32_t>(r8()) <<  0u));
  }

  /** Return a human-readable string representation for this color */
  std::string to_string() const
  {
    return std::to_string(red) + " " + std::to_string(green) + " " + std::to_string(blue);
  }

  SDL_Color to_sdl_color() const
  {
    return { r8(), g8(), b8(), a8() };
  }

  CalculateColor operator+(const Color& o) const { return CalculateColor(red + o.red, green + o.green, blue + o.blue, alpha + o.alpha); }
  CalculateColor operator-(const Color& o) const { return CalculateColor(red - o.red, green - o.green, blue - o.blue, alpha - o.alpha); }
  CalculateColor operator*(const Color& o) const { return CalculateColor(red * o.red, green * o.green, blue * o.blue, alpha * o.alpha); }
  CalculateColor operator/(const Color& o) const { return CalculateColor(red / o.red, green / o.green, blue / o.blue, alpha / o.alpha); }
  CalculateColor operator+(const CalculateColor& o) const { return CalculateColor(red + o.r, green + o.g, blue + o.b, alpha + o.a); }
  CalculateColor operator-(const CalculateColor& o) const { return CalculateColor(red - o.r, green - o.g, blue - o.b, alpha - o.a); }
  CalculateColor operator*(const CalculateColor& o) const { return CalculateColor(red * o.r, green * o.g, blue * o.b, alpha * o.a); }
  CalculateColor operator/(const CalculateColor& o) const { return CalculateColor(red / o.r, green / o.g, blue / o.b, alpha / o.a); }
  CalculateColor operator*(float m) const { return CalculateColor(red * m, green * m, blue * m, alpha * m); }
  CalculateColor operator/(float d) const { return CalculateColor(red / d, green / d, blue / d, alpha / d); }

public:
  float red, green, blue, alpha;
};

#endif

/* EOF */
