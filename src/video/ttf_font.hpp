//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>,
//                     Tobias Markus <tobbi.bugs@googlemail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_TTF_FONT_HPP
#define HEADER_SUPERTUX_VIDEO_TTF_FONT_HPP

#include <SDL_ttf.h>

#include "math/fwd.hpp"
#include "video/color.hpp"
#include "video/font.hpp"

class Canvas;
class Painter;

class TTFFont final : public Font
{
public:
  TTFFont(const std::string& filename, int size, float line_spacing = 1.0f, int shadowsize = 0, int border = 0);
  ~TTFFont() override;

  float get_line_spacing() {
    return m_line_spacing;
  }

  virtual float get_height() const override {
    return static_cast<float>(m_font_size) * m_line_spacing;
  }

  virtual float get_text_width(const std::string& text) const override;
  virtual float get_text_height(const std::string& text) const override;

  virtual std::string wrap_to_width(const std::string& text, float width, std::string* overflow) override;

  virtual Rectf draw_text(Canvas& canvas, const std::string& text,
                          const Vector& pos, FontAlignment alignment, int layer, const Color& color) override;

  inline int get_shadow_size() const { return m_shadow_size; }
  inline int get_border() const { return m_border; }

  inline TTF_Font* get_ttf_font() const { return m_font; }

private:
  TTF_Font* m_font;
  std::string m_filename;
  int m_font_size;
  float m_line_spacing;
  int m_shadow_size;
  int m_border;

private:
  TTFFont(const TTFFont&) = delete;
  TTFFont& operator=(const TTFFont&) = delete;
};

#endif

/* EOF */
