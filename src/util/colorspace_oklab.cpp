// The OKLab colourspace code is licensed under MIT
// Most code is taken from
// https://bottosson.github.io/posts/oklab/#converting-from-linear-srgb-to-oklab
// and
// https://bottosson.github.io/posts/gamutclipping/
//
//  Copyright (c) 2021 Björn Ottosson
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to
//  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//  of the Software, and to permit persons to whom the Software is furnished to do
//  so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.


#include "util/colorspace_oklab.hpp"

#include <algorithm>
#include <cmath>
#include <cfloat>

#include "math/util.hpp"
#include "util/log.hpp"
#include "video/color.hpp"


namespace {

struct ColorRGB {
  float r, g, b;
};

struct ColorOKLab {
  float L, a, b;
};

ColorRGB srgb_to_linear_srgb(const Color& c)
{
  auto to_linear = [&](float channel) -> float {
    if (channel <= 0.04045f)
      return channel / 12.92f;
    else
      return powf((channel + 0.055f) / (1.0f + 0.055f), 2.4f);
  };
  return {to_linear(c.red), to_linear(c.green), to_linear(c.blue)};
}

ColorOKLab linear_srgb_to_oklab(const ColorRGB& c)
{
  float l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
  float m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
  float s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

  float l_ = cbrtf(l);
  float m_ = cbrtf(m);
  float s_ = cbrtf(s);

  return {
    0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_,
    1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
    0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
  };
}

ColorOKLCh lab_to_lch(const ColorOKLab& c)
{
  return ColorOKLCh{c.L, sqrtf(c.a * c.a + c.b * c.b), atan2f(c.b, c.a)};
}

} // namespace


ColorOKLCh::ColorOKLCh(const Color& c) :
  L(0.0f),
  C(0.0f),
  h(0.0f)
{
  ColorRGB rgb = srgb_to_linear_srgb(c);
  ColorOKLab lab = linear_srgb_to_oklab(rgb);
  *this = lab_to_lch(lab);
  if (C < 0.00001f)
    // Deterministic behaviour for greyscale colors
    h = 0.0f;
}

float
ColorOKLCh::get_modified_lightness() const
{
  // The formula is from
  // https://bottosson.github.io/posts/colorpicker/#intermission---a-new-lightness-estimate-for-oklab
  constexpr float k_1 = 0.206f;
  constexpr float k_2 = 0.03f;
  constexpr float k_3 = (1.f + k_1) / (1.f + k_2);
  return 0.5f * (k_3 * L - k_1 + sqrtf((k_3 * L - k_1) * (k_3 * L - k_1)
    + 4 * k_2 * k_3 * L));
}

/* EOF */
